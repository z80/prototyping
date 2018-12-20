﻿using System;
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
using System.Diagnostics;

namespace Isosurface
{
	public enum DrawModes
	{
		Mesh = 1,
		Outline = 2
	}

	public abstract class ISurfaceAlgorithm
	{
		public abstract string Name { get; }

		public GraphicsDevice Device { get; private set; }
		public int Resolution { get; set; }
		public int Size { get; set; }

		public virtual bool Is3D { get; protected set; }
		public virtual bool IsIndexed { get; protected set; }
		public virtual bool CustomWireframe { get; protected set; }
		public virtual bool SpecialShader { get; protected set; }

		public virtual List<VertexPositionColorNormal> Vertices { get; protected set; }
		public List<int> Indices { get; protected set; }

		public DynamicVertexBuffer VertexBuffer { get; set; }
		public DynamicVertexBuffer OutlineBuffer { get; set; }
		public DynamicIndexBuffer IndexBuffer { get; set; }

		public VertexBuffer WireframeBuffer { get; set; }
		public IndexBuffer WireframeIndexBuffer { get; set; }
		public Effect WireframeEffect { get; set; }
		public int WireframeCount { get; set; }
		public int WireframeVertexCount { get; set; }

		public int VertexCount { get; protected set; }
		public int IndexCount { get; protected set; }
		public int OutlineLocation { get; protected set; }

		public virtual string ExtraInformation { get { return ""; } }

		public virtual bool SupportsDeferred { get; protected set; }

		public ISurfaceAlgorithm(GraphicsDevice device, int resolution, int size, bool _3d, bool indexed = true, int vertex_size = 524288, int index_size = 4000000)
		{
			Device = device;
			Resolution = resolution;
			Size = size;

			Is3D = _3d;
			IsIndexed = indexed;
			CustomWireframe = false;
			SpecialShader = false;
			SupportsDeferred = false;

			VertexBuffer = new DynamicVertexBuffer(device, VertexPositionColorNormal.VertexDeclaration, vertex_size, BufferUsage.None);
			OutlineBuffer = new DynamicVertexBuffer(device, VertexPositionColor.VertexDeclaration, index_size, BufferUsage.None);
			if (indexed)
			{
				IndexBuffer = new DynamicIndexBuffer(device, IndexElementSize.ThirtyTwoBits, index_size, BufferUsage.None);
				Indices = new List<int>();
			}

			Vertices = new List<VertexPositionColorNormal>();
		}

		public abstract long Contour(float threshold);

		public virtual void Draw(Effect effect, bool enable_lighting = false, DrawModes mode = DrawModes.Mesh | DrawModes.Outline)
		{
			//effect.LightingEnabled = false;
			if (OutlineLocation > 0 && (mode & DrawModes.Outline) != 0)
			{
				effect.CurrentTechnique.Passes[0].Apply();
				Device.SetVertexBuffer(OutlineBuffer);
				Device.DrawPrimitives(PrimitiveType.LineList, 0, OutlineLocation / 2);
			}

			if ((IsIndexed && IndexCount == 0) || (!IsIndexed && VertexCount == 0) || ((mode & DrawModes.Mesh) == 0))
			{
				Device.SetVertexBuffer(null);
				return;
			}

			if (enable_lighting)
			{
				/*effect.LightingEnabled = true;
				effect.PreferPerPixelLighting = true;
				effect.SpecularPower = 64;
				effect.SpecularColor = Color.Black.ToVector3();
				effect.CurrentTechnique.Passes[0].Apply();
				effect.AmbientLightColor = Color.Gray.ToVector3();*/
			}

			if (effect != null)
				effect.CurrentTechnique.Passes[0].Apply();
			Device.SetVertexBuffer(VertexBuffer);
			if (IsIndexed)
			{
				Device.Indices = IndexBuffer;
				if (Is3D)
					Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, VertexCount, 0, Math.Min(1048575, IndexCount / 3));
				else
					Device.DrawIndexedPrimitives(PrimitiveType.LineList, 0, 0, VertexCount, 0, IndexCount / 2);
				Device.Indices = null;
			}
			else
			{
				if (Is3D)
					Device.DrawPrimitives(PrimitiveType.TriangleList, 0, VertexCount / 3);
				else
					Device.DrawPrimitives(PrimitiveType.LineList, 0, VertexCount / 2);
			}
			Device.SetVertexBuffer(null);
		}

		public virtual void DrawWireframe(Camera c, Effect e, Matrix world)
		{
			if (WireframeCount == 0)
				return;
			e.Parameters["World"].SetValue(world);
			e.Parameters["View"].SetValue(c.View);
			e.Parameters["Projection"].SetValue(c.Projection);
			e.CurrentTechnique.Passes[0].Apply();

			Device.Indices = WireframeIndexBuffer;
			Device.SetVertexBuffer(WireframeBuffer);

			Device.DrawIndexedPrimitives(PrimitiveType.LineList, 0, 0, WireframeVertexCount, 0, WireframeCount / 2);

			Device.SetVertexBuffer(null);
			Device.Indices = null;
		}
	}
}
