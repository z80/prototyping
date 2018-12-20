﻿/* This is my QEF "Solver"
 * As you'll see by the commented out lines and useless Solve2 parameters, I initially tried to port the C++ implementation
 * Lo and behold, I got fed up and decided to bruteforce it
 * The original Dual Contouring paper describes the equation to calculate the error of a point that describes when 3 planes intersect
 * We use this to our advantage and just throw a bunch of pre-calculated points at it and see which one returns the lowest value
 * It's... not efficient, to say the least, but it does preserve both sharp and smooth features
 * Note: It's currently disabled, meaning sharp features aren't preserved
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

namespace Isosurface
{
	public class QEF3D
	{
		public List<Vector3> Intersections { get; set; }
		public List<Vector3> Normals { get; set; }
		public float Error { get; set; }
		Vector3 mass_point;
		private static Random rnd = new Random();
		private static Vector3[] deltas;

		static QEF3D()
		{
			int size = 8;
			deltas = new Vector3[size * size * size];
			for (int x = 0; x < size; x++)
			{
				float dx = (float)x / (float)(size / 2) - 1.0f;
				for (int y = 0; y < size; y++)
				{
					float dy = (float)y / (float)(size / 2) - 1.0f;
					for (int z = 0; z < size; z++)
					{
						float dz = (float)z / (float)(size / 2) - 1.0f;
						deltas[x * size * size + y * size + z] = new Vector3(dx * 0.01f, dy * 0.01f, dz * 0.01f);
					}
				}
			}
		}

		public QEF3D()
		{
			Intersections = new List<Vector3>();
			Normals = new List<Vector3>();
		}

		public void Add(Vector3 p, Vector3 n)
		{
			Intersections.Add(p);
			Normals.Add(n);
			/*ata.M11 += n.X * n.X;
			ata.M12 += n.X * n.Y;
			ata.M13 += 0; //x * z
			ata.M21 += n.Y * n.Y;
			ata.M22 += 0; //y * z
			ata.M23 += 0; //z * z

			float dot = n.X * p.X + n.Y * p.Y;
			atb.X += dot * n.X;
			atb.Y += dot * n.Y;

			btb += dot * dot;*/

			mass_point += p;
		}

		/*public float Solve(out Vector2 p, float svd_tol, int sweeps, float pinv_tol)
		{
			mass_point /= (float)Intersections.Count;
			// this->setAta();
			// this->setAtb();
			//Vec3 tmpv;
			Vector3 tmpv = Vector3.Transform(mass_point, ata);
			//MatUtils::vmul_symmetric(tmpv, this->ata, this->massPoint);
			//  VecUtils::sub(this->atb, this->atb, tmpv);
			atb -= tmpv;
			// this->x.clear();
			x = Vector3.Zero;
			p = new Vector2(mass_point.X, mass_point.Y) / (float)Intersections.Count;
			return 0;
			// const float result = Svd::solveSymmetric(this->ata, this->atb,
			//                       this->x, svd_tol, svd_sweeps, pinv_tol);
			//  VecUtils::addScaled(this->x, 1, this->massPoint);
			// this->setAtb();
			// outx.set(x);
			// this->hasSolution = true;
			// return result;
		}*/

		private float GetDistanceSquared(Vector3 x)
		{
			float total = 0;
			for (int i = 0; i < Intersections.Count; i++)
			{
				Vector3 d = x - Intersections[i];
				float dot = Normals[i].X * d.X + Normals[i].Y * d.Y + Normals[i].Z * d.Z;
				total += dot * dot;
			}
			return total;
		}

		private float GetDistanceSquared(Vector2 x, Vector2 p, Vector2 n)
		{
			Vector2 d = x - p;
			//Vector2 n2 = n * n;
			float dot = n.X * d.X + n.Y * d.Y;
			return dot * dot;
		}

		/* Currently disabled; it just returns the mass point, which means sharp features are lost */
		public Vector3 Solve2(float svd_tol, int sweeps, float pinv_tol)
		{
			if (Intersections.Count == 0)
			{
				this.Error = 100000;
				return Vector3.Zero;
			}
			Vector3 x = mass_point / (float)Intersections.Count;
			float error = GetDistanceSquared(x);
			this.Error = error;
			//return x;

			if (Math.Abs(error) >= 0.0001f)
			{
				for (int i = 0; i < deltas.Length; i++)
				{
					Vector3 new_point = new Vector3(x.X + deltas[i].X, x.Y + deltas[i].Y, x.Z + deltas[i].Z);
					new_point = Vector3.Clamp(new_point, Vector3.Zero, Vector3.One);
					float e = GetDistanceSquared(new_point);
					if (e <= error)
					{
						x = new_point;
						if (Math.Abs(e) < 0.0001f)
							break;
						error = e;
					}
				}
			}

			if (x.X > 1 || x.Y > 1 || x.Z > 1 || x.X < 0 || x.Y < 0 || x.Z < 0)
				return mass_point / (float)Intersections.Count;
			return Vector3.Clamp(x, Vector3.Zero, Vector3.One);
		}
	}
}
