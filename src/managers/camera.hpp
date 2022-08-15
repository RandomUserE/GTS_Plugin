#pragma once
// Module that handles footsteps


using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {
	class CameraManager {
		public:
			[[nodiscard]] static CameraManager& GetSingleton() noexcept;

			void SetfOverShoulderPosX(float value);
			void SetfOverShoulderPosY(float value);
			void SetfOverShoulderPosZ(float value);
			float GetfOverShoulderPosX();
			float GetfOverShoulderPosY();
			float GetfOverShoulderPosZ();

			void Update();

			void OnScaleChanged(float new_scale, float old_scale);

		private:
			float last_scale = -1.0;
	};
}
