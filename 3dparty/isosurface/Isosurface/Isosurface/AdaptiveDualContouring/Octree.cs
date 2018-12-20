﻿/* This is a 3D Adaptive Dual Contouring implementation
 * Dual vertices are placed where they have the smallest error
 * The current QEF solver uses a bruteforce solution when enabled, which is slow and doesn't result in the best-looking meshes
 * But for the most part, it preserves sharp features and smooths round ones
 * It's currently disabled though, so sharp features are NOT preserved
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;
using System.Threading;
using System.Threading.Tasks;

namespace Isosurface.AdaptiveDualContouring
{
	public enum OctreeNodeType
	{
		None,
		Internal,
		Pseudo,
		Leaf
	}

	public class OctreeDrawInfo
	{
		public OctreeDrawInfo()
		{
			index = -1;
			corners = 0;
		}

		public int index;
		public int corners;
		public Vector3 position;
		public Vector3 averageNormal;
		public QEFProper.QEFSolver qef;
	}

	public class OctreeNode
	{
		public OctreeNode()
		{
			type = OctreeNodeType.None;
			position = Vector3.Zero;
			size = 0;
			children = new OctreeNode[8];
			draw_info = null;
		}

		public OctreeNodeType type;
		public Vector3 position;
		public int size;
		public OctreeNode[] children; //Z order
		public OctreeDrawInfo draw_info;

		/* These tables were taken from a scaled-down C++ version of the source that came with the original Dual Contouring paper
		 * They are unformatted and not renamed... yet
		 * AKA: TODO: Rename and format
		 */

		static int[,] edgevmap =
		{
			{0,4},{1,5},{2,6},{3,7},	// x-axis 
			{0,2},{1,3},{4,6},{5,7},	// y-axis
			{0,1},{2,3},{4,5},{6,7}		// z-axis
		};

		static int[] edgemask = { 5, 3, 6 };

		static int[,] vertMap = 
{
	{0,0,0},
	{0,0,1},
	{0,1,0},
	{0,1,1},
	{1,0,0},
	{1,0,1},
	{1,1,0},
	{1,1,1}
};

		static int[,] faceMap = { { 4, 8, 5, 9 }, { 6, 10, 7, 11 }, { 0, 8, 1, 10 }, { 2, 9, 3, 11 }, { 0, 4, 2, 6 }, { 1, 5, 3, 7 } };
		static int[,] cellProcFaceMask = { { 0, 4, 0 }, { 1, 5, 0 }, { 2, 6, 0 }, { 3, 7, 0 }, { 0, 2, 1 }, { 4, 6, 1 }, { 1, 3, 1 }, { 5, 7, 1 }, { 0, 1, 2 }, { 2, 3, 2 }, { 4, 5, 2 }, { 6, 7, 2 } };
		static int[,] cellProcEdgeMask = { { 0, 1, 2, 3, 0 }, { 4, 5, 6, 7, 0 }, { 0, 4, 1, 5, 1 }, { 2, 6, 3, 7, 1 }, { 0, 2, 4, 6, 2 }, { 1, 3, 5, 7, 2 } };

		static int[, ,] faceProcFaceMask = {
	{{4,0,0},{5,1,0},{6,2,0},{7,3,0}},
	{{2,0,1},{6,4,1},{3,1,1},{7,5,1}},
	{{1,0,2},{3,2,2},{5,4,2},{7,6,2}}
};

		static int[, ,] faceProcEdgeMask = {
	{{1,4,0,5,1,1},{1,6,2,7,3,1},{0,4,6,0,2,2},{0,5,7,1,3,2}},
	{{0,2,3,0,1,0},{0,6,7,4,5,0},{1,2,0,6,4,2},{1,3,1,7,5,2}},
	{{1,1,0,3,2,0},{1,5,4,7,6,0},{0,1,5,0,4,1},{0,3,7,2,6,1}}
};

		static int[, ,] edgeProcEdgeMask = {
	{{3,2,1,0,0},{7,6,5,4,0}},
	{{5,1,4,0,1},{7,3,6,2,1}},
	{{6,4,2,0,2},{7,5,3,1,2}},
};

		static int[,] processEdgeMask = { { 3, 2, 1, 0 }, { 7, 5, 6, 4 }, { 11, 10, 9, 8 } };

		public int Build(Vector3 min, int size, float threshold, List<VertexPositionColorNormal> vertices, int grid_size)
		{
			this.position = min;
			this.size = size;
			this.type = OctreeNodeType.Internal;
			int v_index = 0;
			ConstructNodes(vertices, grid_size, 1);
			Simplify(threshold, false);
			return v_index;
		}

		public void GenerateVertexBuffer(List<VertexPositionColorNormal> vertices)
		{
			if (type != OctreeNodeType.Leaf)
			{
				for (int i = 0; i < 8; i++)
				{
					if (children[i] != null)
						children[i].GenerateVertexBuffer(vertices);
				}
			}

			if (type != OctreeNodeType.Internal)
			{
				if (draw_info == null)
					return;

				draw_info.index = vertices.Count;
				Vector3 cn = draw_info.averageNormal * 0.5f + Vector3.One * 0.5f;
				cn.Normalize();
				Color c = new Color(cn);
				vertices.Add(new VertexPositionColorNormal(draw_info.position, c, draw_info.averageNormal));
			}
		}

		/* Threaded is a very simple way to multithread the node generation
		 * Be careful though, as the number of tasks generated is 8^threaded
		 * Anything more than 1 generally is slower than 1
		 * To be safe, either keep it at 0 or 1
		 */
		public bool ConstructNodes(List<VertexPositionColorNormal> vertices, int grid_size, int threaded = 0)
		{
			if (size == 1)
			{
				return ConstructLeaf(vertices, grid_size);
			}

			int child_size = size / 2;
			bool has_children = false;

			Task[] threads = new Task[8];
			bool[] return_values = new bool[8];

			for (int i = 0; i < 8; i++)
			{
				Vector3 offset = new Vector3(i / 4, i % 4 / 2, i % 2);
				OctreeNode child = new OctreeNode();
				child.size = child_size;
				child.position = position + offset * (float)child_size;
				child.type = OctreeNodeType.Internal;

				int index = i;
				if (threaded > 0 && size > 2)
				{
					threads[index] = Task.Factory.StartNew(
						() =>
						{
							return_values[index] = child.ConstructNodes(vertices, grid_size, threaded - 1);
							if (return_values[index])
								children[index] = child;
							else
								children[index] = null;
						}, TaskCreationOptions.AttachedToParent);
					//threads[index].Start();
				}
				else
				{
					if (child.ConstructNodes(vertices, grid_size, 0))
						has_children = true;
					else
						child = null;
					children[i] = child;
				}
			}

			if (threaded > 0 && size > 2)
			{
				for (int i = 0; i < 8; i++)
				{
					threads[i].Wait();
					if (return_values[i])
						has_children = true;
				}
			}

			if (!has_children)
				return false;

			return true;
		}

		/* The following code is more or less a copy/paste cleanup of the C++ implementation
		 * It's not clean, or efficient, but it works and is fairly straightforward
		 */
		public bool ConstructLeaf(List<VertexPositionColorNormal> vertices, int grid_size)
		{
			if (size != 1)
				return false;
			int corners = 0;
			float[, ,] samples = new float[2, 2, 2];
			for (int i = 0; i < 8; i++)
			{
				if ((samples[i / 4, i % 4 / 2, i % 2] = Sampler.Sample(position + new Vector3(i / 4, i % 4 / 2, i % 2))) < 0)
					corners |= 1 << i;
			}

			if (corners == 0 || corners == 255)
				return false;

			//type = OctreeNodeType.Leaf;
			//return true;

			QEF3D qef = new QEF3D();
			QEFProper.QEFSolver qefp = new QEFProper.QEFSolver();
			Vector3 average_normal = Vector3.Zero;
			for (int i = 0; i < 12; i++)
			{
				int c1 = edgevmap[i, 0];
				int c2 = edgevmap[i, 1];

				int m1 = (corners >> c1) & 1;
				int m2 = (corners >> c2) & 1;
				if (m1 == m2)
					continue;

				float d1 = samples[c1 / 4, c1 % 4 / 2, c1 % 2];
				float d2 = samples[c2 / 4, c2 % 4 / 2, c2 % 2];

				Vector3 p1 = new Vector3((float)((c1 / 4)), (float)((c1 % 4 / 2)), (float)((c1 % 2)));
				Vector3 p2 = new Vector3((float)((c2 / 4)), (float)((c2 % 4 / 2)), (float)((c2 % 2)));

				Vector3 intersection = Sampler.GetIntersection(p1, p2, d1, d2) + position;
				Vector3 normal = Sampler.GetNormal(intersection);//GetNormal(x, y);
				average_normal += normal;

				qef.Add(intersection, normal);
				qefp.Add(intersection, normal);
			}

			Vector3 n = average_normal / (float)qef.Intersections.Count;
			n.Normalize();
			draw_info = new OctreeDrawInfo();
			//draw_info.position = position + qef.Solve2(0, 0, 0);
			draw_info.position = qefp.Solve(1e-6f, 4, 1e-6f);
			draw_info.corners = corners;
			draw_info.averageNormal = n;
			draw_info.qef = qefp;
			//vertices.Add(new VertexPositionColorNormal(position + draw_info.position, Color.LightGreen, n));

			type = OctreeNodeType.Leaf;
			return true;
		}

		public void ProcessCell(List<int> indexes)
		{
			if (type == OctreeNodeType.Internal)
			{
				for (int i = 0; i < 8; i++)
				{
					if (children[i] != null)
						children[i].ProcessCell(indexes);
				}

				for (int i = 0; i < 12; i++)
				{
					OctreeNode[] face_nodes = new OctreeNode[2];

					int c1 = cellProcFaceMask[i, 0];
					int c2 = cellProcFaceMask[i, 1];

					face_nodes[0] = children[c1];
					face_nodes[1] = children[c2];

					ProcessFace(face_nodes, cellProcFaceMask[i, 2], indexes);
				}

				for (int i = 0; i < 6; i++)
				{
					OctreeNode[] edge_nodes = 
					{
						children[cellProcEdgeMask[i, 0]],
						children[cellProcEdgeMask[i, 1]],
						children[cellProcEdgeMask[i, 2]],
						children[cellProcEdgeMask[i, 3]]
					};

					ProcessEdge(edge_nodes, cellProcEdgeMask[i, 4], indexes);
				}
			}
		}

		public static void ProcessFace(OctreeNode[] nodes, int direction, List<int> indexes)
		{
			if (nodes[0] == null || nodes[1] == null)
				return;

			if (nodes[0].type == OctreeNodeType.Internal || nodes[1].type == OctreeNodeType.Internal)
			{
				for (int i = 0; i < 4; i++)
				{
					OctreeNode[] face_nodes = new OctreeNode[2];

					for (int j = 0; j < 2; j++)
					{
						if (nodes[j].type != OctreeNodeType.Internal)
							face_nodes[j] = nodes[j];
						else
							face_nodes[j] = nodes[j].children[faceProcFaceMask[direction, i, j]];
					}

					ProcessFace(face_nodes, faceProcFaceMask[direction, i, 2], indexes);
				}

				int[,] orders =
				{
					{ 0, 0, 1, 1 },
					{ 0, 1, 0, 1 },
				};

				for (int i = 0; i < 4; i++)
				{
					OctreeNode[] edge_nodes = new OctreeNode[4];

					for (int j = 0; j < 4; j++)
					{
						if (nodes[orders[faceProcEdgeMask[direction, i, 0], j]].type == OctreeNodeType.Leaf || nodes[orders[faceProcEdgeMask[direction, i, 0], j]].type == OctreeNodeType.Pseudo)
							edge_nodes[j] = nodes[orders[faceProcEdgeMask[direction, i, 0], j]];
						else
							edge_nodes[j] = nodes[orders[faceProcEdgeMask[direction, i, 0], j]].children[faceProcEdgeMask[direction, i, 1 + j]];
					}

					ProcessEdge(edge_nodes, faceProcEdgeMask[direction, i, 5], indexes);
				}
			}
		}

		public static void ProcessEdge(OctreeNode[] nodes, int direction, List<int> indexes)
		{
			if (nodes[0] == null || nodes[1] == null || nodes[2] == null || nodes[3] == null)
				return;

			if (nodes[0].type != OctreeNodeType.Internal && nodes[1].type != OctreeNodeType.Internal && nodes[2].type != OctreeNodeType.Internal && nodes[3].type != OctreeNodeType.Internal)
			{
				ProcessIndexes(nodes, direction, indexes);
			}
			else
			{
				for (int i = 0; i < 2; i++)
				{
					OctreeNode[] edge_nodes = new OctreeNode[4];

					for (int j = 0; j < 4; j++)
					{
						if (nodes[j].type == OctreeNodeType.Leaf || nodes[j].type == OctreeNodeType.Pseudo)
							edge_nodes[j] = nodes[j];
						else
							edge_nodes[j] = nodes[j].children[edgeProcEdgeMask[direction, i, j]];
					}

					ProcessEdge(edge_nodes, edgeProcEdgeMask[direction, i, 4], indexes);
				}
			}
		}

		public static void ProcessIndexes(OctreeNode[] nodes, int direction, List<int> indexes)
		{
			int min_size = 10000000;
			int min_index = 0;
			int[] indices = { -1, -1, -1, -1 };
			bool flip = false;
			bool sign_changed = false;

			for (int i = 0; i < 4; i++)
			{
				int edge = processEdgeMask[direction, i];
				int c1 = edgevmap[edge, 0];
				int c2 = edgevmap[edge, 1];

				int m1 = (nodes[i].draw_info.corners >> c1) & 1;
				int m2 = (nodes[i].draw_info.corners >> c2) & 1;

				if (nodes[i].size < min_size)
				{
					min_size = nodes[i].size;
					min_index = i;
					flip = m1 == 1;
					sign_changed = ((m1 == 0 && m2 != 0) || (m1 != 0 && m2 == 0));
				}

				indices[i] = nodes[i].draw_info.index;

				//sign_changed = true;
			}

			if (sign_changed)
			{
				if (!flip)
				{
					if (indices[0] != indices[1] && indices[1] != indices[3])
					{
						indexes.Add(indices[0]);
						indexes.Add(indices[1]);
						indexes.Add(indices[3]);
					}

					if (indices[0] != indices[2] && indices[2] != indices[3])
					{
						indexes.Add(indices[0]);
						indexes.Add(indices[3]);
						indexes.Add(indices[2]);
					}
				}
				else
				{
					if (indices[0] != indices[1] && indices[1] != indices[3])
					{
						indexes.Add(indices[0]);
						indexes.Add(indices[3]);
						indexes.Add(indices[1]);
					}

					if (indices[0] != indices[2] && indices[2] != indices[3])
					{
						indexes.Add(indices[0]);
						indexes.Add(indices[2]);
						indexes.Add(indices[3]);
					}
				}
			}
		}

		public void Simplify(float threshold, bool randomize = false)
		{
			if (type != OctreeNodeType.Internal)
				return;

			int[] signs = { -1, -1, -1, -1, -1, -1, -1, -1 };
			int mid_sign = -1;
			bool is_collapsible = true;
			//QEF3D qef = new QEF3D();
			QEFProper.QEFSolver qef = new QEFProper.QEFSolver();
			Random rnd = new Random();

			float t = threshold;
			for (int i = 0; i < 8; i++)
			{
				if (children[i] == null)
					continue;

				if (randomize)
					t = (float)rnd.NextDouble() * (float)rnd.NextDouble() * 20.0f;
				children[i].Simplify(t);
				OctreeNode child = children[i];

				if (child.type == OctreeNodeType.Internal)
					is_collapsible = false;
				else
				{
					//qef.Add(child.draw_info.position, child.draw_info.averageNormal);
					//if (child.draw_info.qef != null)
						qef.Add(ref child.draw_info.qef.data);

					mid_sign = (child.draw_info.corners >> (7 - i)) & 1;
					signs[i] = (child.draw_info.corners >> i) & 1;
				}
			}

			if (size == 16)
			{
			}

			if (!is_collapsible)
				return;

			//Vector3 pos = qef.Solve2(0, 0, 0);
			Vector3 pos = qef.Solve(1e-6f, 4, 1e-6f);
			float error = qef.GetError();

			if (error > threshold)
				return;

			OctreeDrawInfo draw_info = new OctreeDrawInfo();

			for (int i = 0; i < 8; i++)
			{
				if (signs[i] == -1)
					draw_info.corners |= mid_sign << i;
				else
					draw_info.corners |= signs[i] << i;
			}

			Vector3 normal = new Vector3();
			for (int i = 0; i < 8; i++)
			{
				if (children[i] != null)
				{
					OctreeNode child = children[i];
					if (child.type == OctreeNodeType.Pseudo || child.type == OctreeNodeType.Leaf)
						normal += child.draw_info.averageNormal;
				}
			}

			normal.Normalize();
			draw_info.averageNormal = normal;
			draw_info.position = pos;
			draw_info.qef = qef;

			for (int i = 0; i < 8; i++)
			{
				children[i] = null;
			}

			type = OctreeNodeType.Pseudo;
			this.draw_info = draw_info;
		}
	}
}
