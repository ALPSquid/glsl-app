#pragma once
#include "Entities/Entity.h"

class MeshUtils {

public:

	/**
	* Creates a torus mesh.
	* Parameter: float outerRadius  Radius of the outer ring.
	* Parameter: float innerRadius  Radius of the inner ring.
	* Parameter: int rings  Number of rings.
	* Parameter: int sides  Number of sides.
	* Returns:   Model  Created torus model.
	*/
	inline static Model createTorus(float outerRadius, float innerRadius, int rings, int sides) {
		float pi_2 = glm::pi<float>() * 2;
		std::vector<Vertex> verts;
		std::vector<GLuint> indices;

		// Based on http://wiki.unity3d.com/index.php/ProceduralPrimitives#C.23_-_Torus
		// Vertices.
		verts.resize((rings + 1) * (sides + 1));
		for (int seg = 0; seg <= rings; seg++) {
			// Wrap back ground if the end has been reached.
			float currSeg = (seg == rings) ? 0 : seg;
			// Angle of the vertex position around the segment.
			float segmentAngle = currSeg / rings * pi_2;
			// Position of the vertex aaround the segment.
			glm::vec3 segmentPosition = glm::vec3(glm::cos(segmentAngle) * outerRadius, 0, glm::sin(segmentAngle) * outerRadius);

			for (int side = 0; side <= sides; side++) {
				float currSide = (side == sides) ? 0 : side;
				// Angle of the vertex around the radius.
				float sideAngle = currSide / sides * pi_2;
				// Position of the vertex along the inner radius of the mesh.
				glm::vec3 sidePosition = glm::angleAxis(-segmentAngle, UP_VECTOR) * glm::vec3(glm::sin(sideAngle) * innerRadius, glm::cos(sideAngle) * innerRadius, 1);

				// Final vertex position is the position around the ring + the position around the segment.
				Vertex vert = Vertex(sidePosition + segmentPosition);
				vert.normal.x = glm::sin(sideAngle);
				vert.normal.y = glm::cos(sideAngle) * glm::sin(segmentAngle);
				vert.normal.z = glm::cos(sideAngle) * glm::cos(segmentAngle);
				vert.texCoords = glm::vec2(currSeg / rings, currSide / sides);
				verts[side + seg * (sides + 1)] = vert;
			}
		}

		// Triangulation.
		int numFaces = verts.size();
		int numTriangles = numFaces * 2;
		int numIndices = numTriangles * 3;
		indices.resize(numIndices);

		int index = 0;
		for (int seg = 0; seg <= rings; seg++) {
			for (int side = 0; side < sides; side++) {
				int current = side + seg * (sides + 1);
				int next = side + ((seg < rings) ? (seg + 1) * (sides + 1) : 0);
				if (index < numIndices - 6) {
					indices[index++] = current;
					indices[index++] = next;
					indices[index++] = next + 1;

					indices[index++] = current;
					indices[index++] = next + 1;
					indices[index++] = current + 1;
				}
			}
		}

		Model torusModel = Model();
		torusModel.addMesh(verts, indices, std::vector<Texture>(), outerRadius);
		return torusModel;
	}
};