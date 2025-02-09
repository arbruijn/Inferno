#pragma once

#include <DirectXMath.h>
#include "Settings.h"

namespace Inferno {
    using DirectX::SimpleMath::Viewport;
    using DirectX::SimpleMath::Quaternion;

    // Descent uses LH coordinate system
    class Camera {
        Vector3 _lerpStart, _lerpEnd;
        float _lerpTime{}, _lerpDuration;

    public:
        Camera() = default;

        Matrix View;
        Matrix Projection;

        float NearClip = 2.0f;
        float FarClip = 3000.0f;

        //Quaternion Rotation;
        Vector3 Position = { 40, 0, 0 };
        Vector3 Target = Vector3::Zero;
        Vector3 Up = Vector3::UnitY;

        const float MinimumZoom = 10; // closest the camera can get to the target
        Viewport Viewport = { 0, 0, 1024, 768, NearClip, FarClip };

        void SetViewport(float width, float height) {
            Viewport = { 0, 0, width, height, NearClip, FarClip };
        }

        void LookAtPerspective(float fovDeg) {
            View = DirectX::XMMatrixLookAtLH(Position, Target, Up);
            Projection = DirectX::XMMatrixPerspectiveFovLH(fovDeg * DegToRad, Viewport.AspectRatio(), Viewport.minDepth, Viewport.maxDepth);
        }

        void LookAtOrthographic() {
            //View = Matrix::CreateLookAt(Position, Target, Up);
            View = DirectX::XMMatrixLookAtLH(Position, Target, Up);
            Projection = Matrix::CreateOrthographicOffCenter(0, Viewport.width, Viewport.height, 0, Viewport.minDepth, Viewport.maxDepth);
        }

        Matrix ViewProj() const {
            return View * Projection;
        }

        void Rotate(float yaw, float pitch) {
            auto qyaw = Quaternion::CreateFromAxisAngle(Up, yaw);
            auto qpitch = Quaternion::CreateFromAxisAngle(GetRight(), pitch);
            //SPDLOG_INFO("Yaw pitch: {}, {}", yaw, pitch);

            Vector3 offset = Target - Position;
            Target = Vector3::Transform(offset, qyaw * qpitch) + Position;
            Up = Vector3::Transform(Up, qpitch);
            Up.Normalize();
        }

        void Roll(float roll) {
            auto qroll = Quaternion::CreateFromAxisAngle(GetForward(), roll * 2);
            Up = Vector3::Transform(Up, qroll);
            Up.Normalize();
        }

        // Orbits around the target point
        void Orbit(float yaw, float pitch) {
            Vector3 offset = Position - Target;
            auto qyaw = Quaternion::CreateFromAxisAngle(Up, yaw);
            auto qpitch = Quaternion::CreateFromAxisAngle(Up.Cross(offset), -pitch);

            Position = Vector3::Transform(offset, qyaw * qpitch) + Target;
            Up = Vector3::Transform(Up, qpitch);
            Up.Normalize();
        }

        Vector3 GetForward() const {
            auto forward = Target - Position;
            forward.Normalize();
            return forward;
        }

        Vector3 GetRight() const {
            auto right = GetForward().Cross(Up);
            right.Normalize();
            return right;
        }

        void Pan(float horizontal, float vertical) {
            auto right = GetRight();
            auto translation = right * horizontal + Up * vertical;
            Target += translation;
            Position += translation;
        }

        void MoveForward(float frameTime) {
            auto step = GetForward() * frameTime;
            Position += step;
            Target += step;
        }

        void MoveBack(float frameTime) {
            auto step = -GetForward() * frameTime;
            Position += step;
            Target += step;
        }

        void MoveLeft(float frameTime) {
            auto step = GetRight() * frameTime;
            Position += step;
            Target += step;
        }

        void MoveRight(float frameTime) {
            auto step = -GetRight() * frameTime;
            Position += step;
            Target += step;
        }

        void MoveUp(float frameTime) {
            auto step = Up * frameTime;
            Position += step;
            Target += step;
        }

        void MoveDown(float frameTime) {
            auto step = -Up * frameTime;
            Position += step;
            Target += step;
        }

        void Zoom(const float& value) {
            Vector3 delta = Target - Position;
            delta.Normalize();
            // add the value along the direction of the vector
            Vector3 pos = Position + delta * value;

            if (Vector3::Distance(pos, Target) > MinimumZoom)
                Position = pos;
        }

        void ZoomIn() {
            auto delta = Target - Position;
            auto direction = delta;
            direction.Normalize();

            // scale zoom amount based on distance from target
            auto value = std::clamp(delta.Length() / 6, 10.0f, 100.0f);
            Vector3 pos = Position + direction * value;

            if (Vector3::Distance(pos, Target) > MinimumZoom)
                Position = pos;
        }

        void ZoomOut() {
            auto delta = Target - Position;
            auto direction = delta;
            direction.Normalize();

            // scale zoom amount based on distance from target
            auto value = std::clamp(delta.Length() / 6, 10.0f, 100.0f);
            Vector3 pos = Position - direction * value;
            Position = pos;
        }

        // Unprojects a screen coordinate into world space along the near plane
        Vector3 Unproject(Vector2 screen, const Matrix& world) const {
            return Viewport.Unproject({ screen.x, screen.y, 0 }, Projection, View, world);
        }

        // Projects a world coordinate into screen space
        Vector3 Project(Vector3 p, const Matrix& world) const {
            return Viewport.Project(p, Projection, View, world);
        }

        Ray UnprojectRay(Vector2 screen, const Matrix& world) const {
            auto direction = Unproject(screen, world) - Position;
            direction.Normalize();
            return Ray(Position, direction);
        }

        // Returns a frustum for the perspective view
        DirectX::BoundingFrustum GetFrustum() const {
            DirectX::BoundingFrustum frustum;
            DirectX::BoundingFrustum::CreateFromMatrix(frustum, Projection);
            DirectX::XMVECTOR s, r, t;
            DirectX::XMMatrixDecompose(&s, &r, &t, View);
            r = DirectX::XMQuaternionInverse(r);
            frustum.Transform(frustum, 1.0f, r, Position);
            return frustum;
        }

        void MoveTo(const Vector3& target) {
            auto translation = target - Target;
            Position += translation;
            Target += translation;
        }

        void LerpTo(const Vector3& target, float duration) {
            _lerpDuration = duration;
            _lerpTime = 0;
            _lerpEnd = target;
            _lerpStart = Target;
        }

        void Update(float dt) {
            if (_lerpTime < _lerpDuration) {
                _lerpTime += dt;
                auto t = _lerpTime / _lerpDuration;
                auto lerp = Vector3::Lerp(_lerpStart, _lerpEnd, t);
                MoveTo(lerp);
            }
        }
    };
}