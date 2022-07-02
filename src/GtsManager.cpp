#include <SKSE/SKSE.h>
#include <Config.h>
#include <GtsManager.h>
#include <vector>
#include <string>

using namespace Gts;
using namespace RE;
using namespace SKSE;
using namespace std;

namespace {
	void update_height(Actor* actor) {
		if (!actor) {
			return;
		}
		if (!actor->Is3DLoaded()) {
			return;
		}

		auto actor_data = GtsManager::GetSingleton().get_actor_extra_data(actor);
		if (actor_data) {
			auto base_actor = actor->GetActorBase();
			auto actor_name = base_actor->GetFullName();

			auto char_controller = actor->GetCharController();
			if (!char_controller) {
				log::info("No char controller: {}", actor_name);
				return;
			}

			bool needs_update = false;
			float prev_scale = char_controller->scale;
			auto size_method = GtsManager::GetSingleton().size_method;
			float scale = GtsManager::GetSingleton().test_scale;
			// If zero we are not using the test scales.
			// We reply on the gts mod to manage them
			if (scale < 1e-5) {
				scale = get_scale(actor);
				if (fabs(prev_scale - scale) > 1e-5) {
					needs_update = true;
				}
			} else {
				// If non zero we set the scales ourself
				float current_scale = get_scale(actor);
				if (fabs(prev_scale - scale) > 1e-5 || fabs(current_scale - scale) > 1e-5) {
					if (!set_scale(actor, scale)) {
						log::info("Unable to set scale");
						needs_update = true;
						return;
					}
				}
			}
			if (scale <= 1e-5) {
				return;
			}

			if (needs_update) {
				// Get base data
				auto& base_height_data = actor_data->base_height;

				log::info("Updating height of: {}", actor_name);

				//if (!actor_data->initialised) {
				//	clone_bound(actor);
				//}
				auto bsbound = get_bound(actor);
				if (!bsbound) {
					log::info("No bound: {}", actor_name);
					return;
				}
				auto model = actor->Get3D();
				if (!model) {
					log::info("No model: {}", actor_name);
					return;
				}
				auto ai_process = actor->currentProcess;

				// Start
				log::info("Current scale: {}", prev_scale);
				log::info("Current Bounding box: {},{},{}", bsbound->extents.x, bsbound->extents.y, bsbound->extents.z);
				log::info("Current Bound min: {},{},{}", actor->GetBoundMin().x, actor->GetBoundMin().y, actor->GetBoundMin().z);
				log::info("Current Bound max: {},{},{}", actor->GetBoundMax().x, actor->GetBoundMax().y, actor->GetBoundMax().z);

				if (size_method == SizeMethod::RootScale) {
					// Root scale is the only one that dosent update the bumper
					// itself, so we do it manually
					auto bumper = get_bumper(actor);
					if (bumper) {
						bumper->local.translate = base_height_data.bumper_transform.translate * scale;
						bumper->local.scale = base_height_data.bumper_transform.scale * scale;
					}
				}

				// Character controller stuff
				char_controller->scale = scale;
				uncache_bound(&base_height_data.collisionBound, &char_controller->collisionBound);
				char_controller->collisionBound.extents *= scale;
				char_controller->collisionBound.center *= scale;
				uncache_bound(&base_height_data.bumperCollisionBound, &char_controller->bumperCollisionBound);
				char_controller->bumperCollisionBound.extents *= scale;
				char_controller->bumperCollisionBound.center *= scale;
				uncache_bound(&base_height_data.collisionBound, bsbound);
				bsbound->extents *= scale;
				bsbound->center *= scale;
				actor->UpdateCharacterControllerSimulationSettings(*char_controller);

				// Ai Proccess stuff
				float model_height = bsbound->extents.z * 2 * actor->GetBaseHeight();
				float meter_height = unit_to_meter(model_height);
				char_controller->actorHeight = meter_height;
				char_controller->swimFloatHeight = meter_height * base_height_data.swimFloatHeightRatio;
				if (ai_process) {
					ai_process->SetCachedHeight(model_height);
					ai_process->cachedValues->cachedEyeLevel = model_height * 0.95;
				}
				log::info("Data updated");

				// 3D resets
				if (ai_process) {
					ai_process->Update3DModel(actor);
				} else {
					log::info("No ai: {}", actor_name);
				}
				actor->Update3DModel();
				auto task = SKSE::GetTaskInterface();
				auto node = actor->Get3D();
				task->AddTask([node]() {
					if (node) {
						log::info("Updating world models data on main thread");
						NiUpdateData ctx;
						ctx.flags |= NiUpdateData::Flag::kDirty;
						node->UpdateWorldData(&ctx);
						node->UpdateWorldBound();
						node->UpdateRigidDownwardPass(ctx, 100);
						node->UpdateDownwardPass(ctx, 100);
						node->UpdateRigidConstraints(true);
					}
				});

				// Done
				float current_scale = get_scale(actor);

				log::info("New scale: {}", current_scale);
				log::info("New Bounding box: {},{},{}", bsbound->extents.x, bsbound->extents.y, bsbound->extents.z);
				log::info("New Bound min: {},{},{}", actor->GetBoundMin().x, actor->GetBoundMin().y, actor->GetBoundMin().z);
				log::info("New Bound max: {},{},{}", actor->GetBoundMax().x, actor->GetBoundMax().y, actor->GetBoundMax().z);
				actor_data->initialised = true;
			}

			//

			// auto char_controller = actor->GetCharController();
			// if (char_controller) {
			// 	// Havok Direct SCALE
			// 	hkTransform fill_me;
			// 	auto char_controller_transform = char_controller->GetTransformImpl(fill_me);
			// 	auto translation = char_controller_transform.translation;
			// 	float output[4];
			// 	_mm_storeu_ps(&output, translation);
			// 	log::info("Char Controler transform: pos={},{},{},{}", output[0], output[1], output[2], output[3]);
			// 	// Time to cheat the transform
			// 	// There's no scale on the hk transform there are two ways to get it
			// 	// 1. Put the scale in the w of the translation
			// 	// 2. Put the scale in the cross diagnoal of the rotation
			// 	auto multi = _mm_set_ps(1.0, 1.0, 1.0, scale);
			// 	auto result = _mm_mul_ps(translation, multi);
			// 	char_controller_transform.translation = result;
			// }
		}
	}
}

GtsManager& GtsManager::GetSingleton() noexcept {
	static GtsManager instance;

	static std::atomic_bool initialized;
	static std::latch latch(1);
	if (!initialized.exchange(true)) {
		instance.test_scale = Gts::Config::GetSingleton().GetTest().GetScale();
		instance.size_method = SizeMethod::All;
		latch.count_down();
	}
	latch.wait();

	return instance;
}

// Poll for updates
void GtsManager::poll() {
	auto player_char = RE::PlayerCharacter::GetSingleton();
	if (!player_char) {
		return;
	}
	if (!player_char->Is3DLoaded()) {
		return;
	}

	auto ui = RE::UI::GetSingleton();
	if (!ui->GameIsPaused()) {
		const auto& frame_config = Gts::Config::GetSingleton().GetFrame();
		auto init_delay = frame_config.GetInitDelay();
		auto step = frame_config.GetStep() + 1; // 1 Based index

		auto current_frame = this->frame_count.fetch_add(1);
		if (current_frame < init_delay) {
			return;
		}
		if ((current_frame - init_delay) % step != 0) {
			return;
		}

		// Key presses
		auto keyboard = this->get_keyboard();
		if (keyboard) {
			if (keyboard->IsPressed(BSKeyboardDevice::Keys::kBracketLeft)) {
				log::info("Size UP");
				this->test_scale += 0.1;
			}
			else if (keyboard->IsPressed(BSKeyboardDevice::Keys::kBracketRight)) {
				log::info("Size Down");
				this->test_scale -= 0.1;
			}
			if (this->test_scale < 0.0) {
				this->test_scale = 0.0; // 0.is disabled
			}
		} else {
			log::info("No keyboard!");
		}
	}
}

void GtsManager::poll_actor(Actor* actor) {
	if (actor) {
		auto base_actor = actor->GetActorBase();
		auto actor_name = base_actor->GetFullName();

		auto race = actor->GetRace();
		auto race_name = race->GetFullName();


		// log::trace("Updating height of {}", actor_name);
		update_height(actor);
		// walk_nodes(actor);
	}
}

ActorExtraData* GtsManager::get_actor_extra_data(Actor* actor) {
	auto key = actor->GetFormID();
	try {
		auto no_discard = this->actor_data.at(key);
	} catch (const std::out_of_range& oor) {
		// Try to add
		log::info("Init bounding box");
		ActorExtraData result;

		auto bsbound = get_bound(actor);
		if (!bsbound) {
			return nullptr;
		}
		auto char_controller = actor->GetCharController();
		if (!char_controller) {
			return nullptr;
		}

		cache_bound(bsbound, &result.base_height.collisionBound);
		cache_bound(&char_controller->bumperCollisionBound, &result.base_height.bumperCollisionBound);
		result.base_height.actorHeight = char_controller->actorHeight;
		result.base_height.swimFloatHeightRatio = char_controller->swimFloatHeight / char_controller->actorHeight;
		auto bumper = get_bumper(actor);
		if (bumper) {
			result.base_height.bumper_transform = bumper->local;
		}
		result.initialised = false;
		this->actor_data[key] = result;
	}
	return &this->actor_data[key];
}

void Gts::cache_bound(BSBound* src, CachedBound* dst) {
	dst->center[0] = src->center.x;
	dst->center[1] = src->center.y;
	dst->center[2] = src->center.z;
	dst->extents[0] = src->extents.x;
	dst->extents[1] = src->extents.y;
	dst->extents[2] = src->extents.z;
}
void Gts::uncache_bound(CachedBound* src, BSBound* dst) {
	dst->center.x = src->center[0];
	dst->center.y = src->center[1];
	dst->center.z = src->center[2];
	dst->extents.x = src->extents[0];
	dst->extents.y = src->extents[1];
	dst->extents.z = src->extents[2];
}

BSWin32KeyboardDevice* GtsManager::get_keyboard() {
	if (!this->keyboard) {
		auto input_manager = BSInputDeviceManager::GetSingleton();
		if (input_manager) {
			this->keyboard = input_manager->GetKeyboard();
		}
	}
	return this->keyboard;
}
