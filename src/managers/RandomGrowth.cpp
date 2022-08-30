#include "managers/RandomGrowth.hpp"
#include "managers/GtsManager.hpp"
#include "util.hpp"
#include "scale/scale.hpp"
#include "data/runtime.hpp"

using namespace RE;
using namespace Gts;

namespace {
	bool ShouldGrow() {
		//if (GtsManager::GetSingleton().GetFrameNum() % 120) {
		auto& runtime = Runtime::GetSingleton();
		int random = rand() % 2400 + 1;
		int decide_chance = 1;
		auto GrowthPerk = runtime.GrowthPerk;
		auto Player = PlayerCharacter::GetSingleton();
		if (random <= decide_chance && Player->HasPerk(GrowthPerk)) {
			return true;
		} else {
			return false;
		}
	}
}

namespace Gts {
	RandomGrowth& RandomGrowth::GetSingleton() noexcept {
		static RandomGrowth instance;
		return instance;
	}

	void RandomGrowth::Update() {
		auto player = PlayerCharacter::GetSingleton();
		if (!player) {
			return;
		}
		if (!player->Is3DLoaded()) {
			return;
		}

		log::info("Growing {}", ShouldGrow());
		log::info("Is Growing: {}", this->state == State::Working);

		switch (this->state) {
			case State::Idle: {
				if (ShouldGrow()) {
					// Start growing state
					this->state = State::Working;
					this->growth_time = 0.0;

					// Play sound
					auto& runtime = Runtime::GetSingleton();
					auto MoanSound = runtime.MoanSound;
					auto GrowthSound = runtime.growthSound;
					float Volume = clamp(0.25, 1.0, get_visual_scale(Player)/4);
					PlaySound(MoanSound, player, Volume);
					PlaySound(GrowthSound, player, Volume);
				}
			}
			case State::Working: {
				// Do the growing
				auto& runtime = Runtime::GetSingleton();
				float delta_time = *g_delta_time;
				float Scale = get_visual_scale(player);
				float ProgressionMultiplier = runtime.ProgressionMultiplier->value;
				float base_power = ((0.0025 * 60.0 * Scale) * ProgressionMultiplier);  // Put in actual power please
				mod_target_scale(player, base_power * delta_time); // Use delta_time so that the growth will be the same regardless of fps

				this->growth_time += delta_time;
				if (this->growth_time >= 2.6) { // Time in seconds" 160tick / 60 ticks per secong ~= 2.6s
					// End growing
					this->state = State::Idle;
				}
			}
		}
	}
}
