#include "pch.h"
#include "Collision_Legacy.h"
#include "Log.h"

#include <algorithm>
#include <functional>

namespace Engine
{
	bool Collision::Intersects(Collider* colliderA, Collider* colliderB, Contact& contact)
	{
        const ColliderType typeA = colliderA->GetColliderType();
        const ColliderType typeB = colliderB->GetColliderType();

        // box to box
        if (typeA == ColliderType::OBB && typeB == ColliderType::OBB)
        {
            return IntersectsBoxToBox(static_cast<BoxCollider*>(colliderA), static_cast<BoxCollider*>(colliderB), contact);
        }

        // box to sphere
        if (typeA == ColliderType::OBB && typeB == ColliderType::Sphere)
        {
            return IntersectsBoxToSphere(static_cast<BoxCollider*>(colliderA), static_cast<SphereCollider*>(colliderB), contact);
        }
        if (typeA == ColliderType::Sphere && typeB == ColliderType::OBB)
        {
            return IntersectsBoxToSphere(static_cast<BoxCollider*>(colliderB), static_cast<SphereCollider*>(colliderA), contact);
        }

        // sphere to sphere
        if (typeA == ColliderType::Sphere && typeB == ColliderType::Sphere)
        {
            return IntersectsSphereToSphere(static_cast<SphereCollider*>(colliderA), static_cast<SphereCollider*>(colliderB), contact);
        }

        return false;
	}

    bool Collision::IntersectsBoxToBox(BoxCollider* boxA, BoxCollider* boxB, Contact& contact)
    {
        // build OBB data
        OBBData obbDataA = BuildOBBData(boxA);
        OBBData obbDataB = BuildOBBData(boxB);

        // Check intersects with Separating Axis Theorem
        float minOverlap = FLT_MAX;
        Vector3 bestAxis;
        Vector3 centerDelta = obbDataA.center - obbDataB.center;
        OBBContactType contactType = OBBContactType::Face;
        bool isSparated = false;
        auto CalculateBestAxis = [&](const Vector3& normal)
            {
                // distance between centers about axis
                float distance = std::abs(Dot(centerDelta, normal));
                // radius about axis
                float radiusA = std::abs(Dot(obbDataA.axis[0], normal)) * obbDataA.extents.x + std::abs(Dot(obbDataA.axis[1], normal)) * obbDataA.extents.y + std::abs(Dot(obbDataA.axis[2], normal)) * obbDataA.extents.z;
                float radiusB = std::abs(Dot(obbDataB.axis[0], normal)) * obbDataB.extents.x + std::abs(Dot(obbDataB.axis[1], normal)) * obbDataB.extents.y + std::abs(Dot(obbDataB.axis[2], normal)) * obbDataB.extents.z;

                // check intersect
                float overlap = radiusA + radiusB - distance;
                if (overlap < 0.0f)
                {
                    isSparated = true;
                    return false;
                }
                // update physics info
                if (overlap < minOverlap)
                {
                    minOverlap = overlap;
                    bestAxis = normal;
                    return true;
                }
                return false;
            };
        // check obbA face contact
        for (int i = 0; i < 3; ++i)
        {
            // Ignores zero vector operations
            if (LengthSq(obbDataA.axis[i]) >= 1e-6f)
            {
                Vector3 normalizedAxis = Normalize(obbDataA.axis[i]);
                if (CalculateBestAxis(normalizedAxis))
                {
                    contactType = OBBContactType::Face;
                }

                if (isSparated) return false;
            }
        }
        // check obbB face contact
        for (int i = 0; i < 3; ++i)
        {
            // Ignores zero vector operations
            if (LengthSq(obbDataB.axis[i]) >= 1e-6f)
            {
                Vector3 normalizedAxis = Normalize(obbDataB.axis[i]);
                if (CalculateBestAxis(normalizedAxis))
                {
                    contactType = OBBContactType::Face;
                }

                if (isSparated) return false;
            }
        }
        // check edge contact
        AxisType axisAType = AxisType::Right;
        AxisType axisBType = AxisType::Right;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                // get cross axis
                Vector3 axis = Cross(obbDataA.axis[i], obbDataB.axis[j]);
                // Ignores zero vector operations
                if (LengthSq(axis) >= 1e-6f)
                {
                    Vector3 normalizedAxis = Normalize(axis);
                    if (CalculateBestAxis(normalizedAxis))
                    {
                        contactType = OBBContactType::Edge;
                        axisAType = static_cast<AxisType>(i);
                        axisBType = static_cast<AxisType>(j);
                    }

                    if (isSparated) return false;
                }
            }
        }

        if (Dot(centerDelta, bestAxis) < 0.0f)
        {
            bestAxis = { -bestAxis.x, -bestAxis.y, -bestAxis.z };
        }

        // calculate contact point
        if (contactType == OBBContactType::Face)    // face - face
        {
            auto manifoldPoints = BuildFaceManifold(obbDataA, obbDataB, bestAxis);
            
            if (manifoldPoints.empty())
            {
                return false;
            }

            for (int i = 0; i < manifoldPoints.size(); ++i)
            {
                contact.contactPoints[i] = manifoldPoints[i].point;
                contact.penetrations[i] = minOverlap;
                contact.ids[i] = manifoldPoints[i].id;
            }
            contact.pointCount = manifoldPoints.size();
        }
        else   // edge - edge
        {
            return false;
        }

        contact.normal = bestAxis;

        contact.pair.a = boxA;
        contact.pair.b = boxB;

        return true;
	}

	bool Collision::IntersectsBoxToSphere(BoxCollider* box, SphereCollider* sphere, Contact& contact)
	{
        const Vector3 boxCenter = box->GetWorldCenter();
        const Vector3 extents = box->GetWorldExtents();

        const Vector3 sphereCenter = sphere->GetWorldCenter();
        const float sphereRadius = sphere->GetWorldRadius();

        const Vector3 right = box->ownerGameObject->transform.GetRight();
        const Vector3 up = box->ownerGameObject->transform.GetUp();
        const Vector3 forward = box->ownerGameObject->transform.GetForward();

        // Sphere center relative to OBB center
        const Vector3 d = sphereCenter - boxCenter;

        // Convert sphere center to OBB local coordinates
        const float localX = Dot(d, right);
        const float localY = Dot(d, up);
        const float localZ = Dot(d, forward);

        // Closest point on box in local coordinates
        const float closestX = std::clamp(localX, -extents.x, extents.x);
        const float closestY = std::clamp(localY, -extents.y, extents.y);
        const float closestZ = std::clamp(localZ, -extents.z, extents.z);

        // Closest point back to world
        uint32_t pointCount = 0;
        const Vector3 closestPoint = boxCenter + right * closestX + up * closestY + forward * closestZ;
        contact.contactPoints[pointCount] = closestPoint;

        Vector3 delta = closestPoint - sphereCenter;
        float distanceSq = LengthSq(delta);
        if (distanceSq > sphereRadius * sphereRadius) return false;

        float distance = std::sqrt(distanceSq);
        if (distance > 1e-6f)
        {
            contact.normal = delta / distance;
            contact.penetrations[pointCount] = sphereRadius - distance;
        }
        else
        {
            contact.normal = Normalize(boxCenter - sphereCenter);
            contact.penetrations[pointCount] = sphereRadius;
        }

        contact.ids[pointCount] = 0;
        contact.pointCount = ++pointCount;

        contact.pair.a = box;
        contact.pair.b = sphere;

        return true;
	}

	bool Collision::IntersectsSphereToSphere(SphereCollider* sphereA, SphereCollider* sphereB, Contact& contact)
	{
        const Vector3 centerA = sphereA->GetWorldCenter();
        const Vector3 centerB = sphereB->GetWorldCenter();

        const float radiusA = sphereA->GetWorldRadius();
        const float radiusB = sphereB->GetWorldRadius();

        Vector3 delta = centerA - centerB;
        float distanceSq = LengthSq(delta);
        const float radiusSum = radiusA + radiusB;

        if (distanceSq > radiusSum * radiusSum) return false;

        uint32_t pointCount = 0;
        float distance = std::sqrt(distanceSq);
        if (distance > 1e-6f)
        {
            contact.normal = delta / distance;
            contact.penetrations[pointCount] = radiusSum - distance;
            contact.contactPoints[pointCount] = centerA + contact.normal * radiusA;
        }
        else
        {
            contact.normal = Vector3(1, 0, 0);
            contact.penetrations[pointCount] = radiusSum;
            contact.contactPoints[pointCount] = centerA;
        }
        
        contact.ids[pointCount] = 0;
        contact.pointCount = ++pointCount;

        contact.pair.a = sphereA;
        contact.pair.b = sphereB;

        return true;
	}

    OBBData Collision::BuildOBBData(BoxCollider* box)
    {
        OBBData data{ };
        data.center = box->GetWorldCenter();
        // colliders world extents
        data.extents = box->GetWorldExtents();
        // colliderA axes
        data.axis[0] = box->ownerGameObject->transform.GetRight();
        data.axis[1] = box->ownerGameObject->transform.GetUp();
        data.axis[2] = box->ownerGameObject->transform.GetForward();

        data.id = box->GetColliderID();

        return data;
    }

    std::vector<ManifoldPoint> Collision::BuildFaceManifold(const OBBData& obbA, const OBBData& obbB, const Vector3& collisionNormal)
    {
        float scoreA = 0;
        float scoreB = 0;

        AxisType bestAxisAType = AxisType::Right;
        AxisType bestAxisBType = AxisType::Right;

        // search face and axis
        // A
        for (int i = 0; i < 3; ++i)
        {
            float d = std::abs(Dot(collisionNormal, obbA.axis[i]));
            if (d > scoreA)
            {
                scoreA = d;
                bestAxisAType = static_cast<AxisType>(i);
            }
        }
        // B
        for (int i = 0; i < 3; ++i)
        {
            float d = std::abs(Dot(collisionNormal, obbB.axis[i]));
            if (d > scoreB)
            {
                scoreB = d;
                bestAxisBType = static_cast<AxisType>(i);
            }
        }

        // The obb with the face most parallel to the collision normal becomes the reference.
        bool useAAsReference;
        if (std::abs(scoreA - scoreB) < 1e-4f)
        {
            useAAsReference = obbA.id < obbB.id ? true : false;
        }
        else
        {
            useAAsReference = (scoreA > scoreB);
        }

        const OBBData& refData = useAAsReference ? obbA : obbB;
        const OBBData& incData = useAAsReference ? obbB : obbA;

        // reference and incident data
        const Vector3& refCenter = refData.center;
        const Vector3& refExtents = refData.extents;
        const Vector3& refRight = refData.axis[0];
        const Vector3& refUp = refData.axis[1];
        const Vector3& refForward = refData.axis[2];

        const Vector3& incCenter = incData.center;
        const Vector3& incExtents = incData.extents;
        const Vector3& incRight = incData.axis[0];
        const Vector3& incUp = incData.axis[1];
        const Vector3& incForward = incData.axis[2];

        // Collision surface candidate structure
        struct FaceCandidate
        {
            Vector3 normal; // face normal
            int axis;   // axis index
            bool positive;  // normal direction
        };

        FaceCandidate faces[6] =
        {
            { incRight, 0, true },
            { incRight * -1.0f, 0, false },

            { incUp, 1, true },
            { incUp * -1.0f, 1, false },

            { incForward, 2, true },
            { incForward * -1.0f, 2, false }
        };

        // Find the face most vertical to the collision normal.
        float minDot = FLT_MAX;
        FaceCandidate incidentFace;
        for (auto& face : faces)
        {
            float d = Dot(face.normal, collisionNormal);

            if (d < minDot)
            {
                minDot = d;
                incidentFace = face;
            }
        }
        // Generates vertices for the selected face
        std::array<Vector3, 4> incidentVerts;
        Vector3 R = incRight * incExtents.x;
        Vector3 U = incUp * incExtents.y;
        Vector3 F = incForward * incExtents.z;
        switch (incidentFace.axis)
        {
            case 0:
            {
                Vector3 X = incidentFace.positive ? R : R * -1.0f;

                incidentVerts =
                {
                    incCenter + X + U + F,
                    incCenter + X - U + F,
                    incCenter + X - U - F,
                    incCenter + X + U - F
                };
                break;
            }
            case 1:
            {
                Vector3 Y = incidentFace.positive ? U : U * -1.0f;

                incidentVerts =
                {
                    incCenter + Y + R + F,
                    incCenter + Y - R + F,
                    incCenter + Y - R - F,
                    incCenter + Y + R - F
                };
                break;
            }
            case 2:
            {
                Vector3 Z = incidentFace.positive ? F : F * -1.0f;

                incidentVerts =
                {
                    incCenter + Z + R + U,
                    incCenter + Z - R + U,
                    incCenter + Z - R - U,
                    incCenter + Z + R - U
                };
                break;
            }
        }


        int bestRefAxisIndex = useAAsReference ? static_cast<int>(bestAxisAType) : static_cast<int>(bestAxisBType);
        Vector3 refFaceNormal = refData.axis[bestRefAxisIndex];
        if (Dot(refFaceNormal, collisionNormal) < 0.0f)
        {
            refFaceNormal *= -1.0f;
        }

        int refAxisIndex = bestRefAxisIndex;

        // Find the center point of the collision surface.
        Vector3 refFaceCenter;
        switch (refAxisIndex)
        {
            case 0:
            {
                float sign = Dot(refFaceNormal, refRight) > 0 ? 1.0f : -1.0f;

                refFaceCenter = refCenter + refRight * refExtents.x * sign;
                break;
            }

            case 1:
            {
                float sign = Dot(refFaceNormal, refUp) > 0 ? 1.0f : -1.0f;

                refFaceCenter = refCenter + refUp * refExtents.y * sign;
                break;
            }

            case 2:
            {
                float sign = Dot(refFaceNormal, refForward) > 0 ? 1.0f : -1.0f;

                refFaceCenter = refCenter + refForward * refExtents.z * sign;
                break;
            }
        }

        // Get two axes, width, and height for clipping.
        Vector3 tangent1;
        Vector3 tangent2;

        float extent1;
        float extent2;

        switch (refAxisIndex)
        {
        case 0:
            tangent1 = refUp;
            tangent2 = refForward;

            extent1 = refExtents.y;
            extent2 = refExtents.z;
            break;

        case 1:
            tangent1 = refRight;
            tangent2 = refForward;

            extent1 = refExtents.x;
            extent2 = refExtents.z;
            break;

        case 2:
            tangent1 = refRight;
            tangent2 = refUp;

            extent1 = refExtents.x;
            extent2 = refExtents.y;
            break;
        }

        // Get plane to clip the incident vertex.
        struct Plane
        {
            Vector3 normal;
            float distance;
        };

        Plane clipPlanes[4];

        clipPlanes[0].normal = tangent1;
        clipPlanes[0].distance = Dot(tangent1, refFaceCenter + tangent1 * extent1);

        clipPlanes[1].normal = tangent1 * -1.0f;
        clipPlanes[1].distance = Dot(tangent1 * -1.0f, refFaceCenter - tangent1 * extent1);

        clipPlanes[2].normal = tangent2;
        clipPlanes[2].distance = Dot(tangent2, refFaceCenter + tangent2 * extent2);

        clipPlanes[3].normal = tangent2 * -1.0f;
        clipPlanes[3].distance = Dot(tangent2 * -1.0f, refFaceCenter - tangent2 * extent2);

        struct ClipVertex
        {
            Vector3 position;

            uint8_t v0;
            uint8_t v1;
        };
        std::vector<ClipVertex> polygon =
        {
            { incidentVerts[0], 0, 0 },
            { incidentVerts[1], 1, 1 },
            { incidentVerts[2], 2, 2 },
            { incidentVerts[3], 3, 3 }
        };
        auto ClipPolygonAgainstPlane = [](const std::vector<ClipVertex>& polygon, const Plane& plane)
            {
                std::vector<ClipVertex> result;

                if (polygon.empty())
                    return result;

                for (size_t i = 0; i < polygon.size(); i++)
                {
                    const auto& A = polygon[i];
                    const auto& B = polygon[(i + 1) % polygon.size()];

                    // Check if the vertex is inside the plane.
                    float distA = Dot(plane.normal, A.position) - plane.distance;
                    float distB = Dot(plane.normal, B.position) - plane.distance;

                    bool insideA = distA <= 0.0f;
                    bool insideB = distB <= 0.0f;

                    if (insideA && insideB) // If both points are inside, point a is already included.
                    {
                        result.push_back(A);
                    }
                    else if (insideA && !insideB)
                    {
                        result.push_back(A);

                        float t = distA / (distA - distB);
                        ClipVertex V;
                        V.position = A.position + (B.position - A.position) * t;
                        V.v0 = A.v0;
                        V.v1 = B.v0;

                        result.push_back(V);
                    }
                    else if (!insideA && insideB)   // only point B is inside
                    {
                        float t = distA / (distA - distB);
                        ClipVertex V;
                        V.position = A.position + (B.position - A.position) * t;

                        // edge(A,B)에서 생성된 교점
                        V.v0 = A.v0;
                        V.v1 = B.v0;

                        result.push_back(V);
                        result.push_back(B);
                    }
                }

                return result;
            };
        for (int i = 0; i < 4; i++)
        {
            polygon = ClipPolygonAgainstPlane(polygon, clipPlanes[i]);
        }

        // { -x, +x, -y, +y, -z, +z }
        int refFaceIndex = bestRefAxisIndex * 2 + (Dot(refFaceNormal, refData.axis[bestRefAxisIndex]) > 0.0f ? 1 : 0);
        int incFaceIndex = incidentFace.axis * 2 + (incidentFace.positive ? 1 : 0);

        std::vector<ManifoldPoint> manifoldPoints;
        for (auto& p : polygon)
        {
            float d = Dot(refFaceNormal, p.position - refFaceCenter);
            if (d <= 1e-3f)
            {
                Vector3 projected = p.position - refFaceNormal * Dot(refFaceNormal, p.position - refFaceCenter);

                ManifoldPoint mp;
                mp.point = projected;
                mp.id = refFaceIndex | (incFaceIndex << 3) | (p.v0 << 6) | (p.v1 << 8);

                manifoldPoints.push_back(mp);
            }
        }

        return manifoldPoints;
    }

    Vector3 Collision::BuildEdgeContact(const OBBData& obbA, const OBBData& obbB, int axisAIndex, int axisBIndex, const Vector3& collisionNormal)
    {
        return Vector3();
    }
}