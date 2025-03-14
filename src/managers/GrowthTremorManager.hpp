#pragma once
// Module that handles AttributeValues


using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {

	class GrowthTremorManager {
		public:
			[[nodiscard]] static GrowthTremorManager& GetSingleton() noexcept;

			void CallRumble(Actor* Source, Actor* Receiver, float Modifier);
            inline float ShakeStrength(Actor* Source);
	};
}