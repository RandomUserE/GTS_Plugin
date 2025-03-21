#include "magic/magic.hpp"
#include "util.hpp"
#include "magic/effects/common.hpp"
#include "magic/effects/absorb_effect.hpp"
#include "magic/effects/explosive_growth.hpp"
#include "magic/effects/EnchGigantism.hpp"
#include "magic/effects/grow_button.hpp"
#include "magic/effects/grow_other.hpp"
#include "magic/effects/tracksize.hpp"
#include "magic/effects/grow_other_button.hpp"
#include "magic/effects/growth.hpp"
#include "magic/effects/CrushGrowth.hpp"
#include "magic/effects/shrink.hpp"
#include "magic/effects/shrink_back.hpp"
#include "magic/effects/shrink_back_other.hpp"
#include "magic/effects/shrink_button.hpp"
#include "magic/effects/shrink_foe.hpp"
#include "magic/effects/shrink_other.hpp"
#include "magic/effects/shrink_other_button.hpp"
#include "magic/effects/slow_grow.hpp"
#include "magic/effects/sword_of_size.hpp"
#include "magic/effects/vore_growth.hpp"
#include "magic/effects/SizeRelatedDamage.hpp"
#include "magic/effects/SmallMassiveThreat.hpp"
#include "magic/effects/GrowthPotion.hpp"
#include "magic/effects/SizePotion.hpp"
#include "magic/effects/sizestealpotion.hpp"
#include "managers/Attributes.hpp"
#include "data/runtime.hpp"

namespace Gts {

	void Magic::OnStart() {
	}
	void Magic::OnUpdate() {
	}
	void Magic::OnFinish() {
	}

	std::string Magic::GetName() {
		return "";
	}


	Magic::Magic(ActiveEffect* effect) : activeEffect(effect) {
		if (this->activeEffect) {
			this->effectSetting = this->activeEffect->GetBaseObject();
			MagicTarget* m_target = this->activeEffect->target;
			if (m_target) {
				if (m_target->MagicTargetIsActor()) {
					this->target = skyrim_cast<Actor*>(m_target);
				}
			}
			if (this->activeEffect->caster) {
				this->caster = this->activeEffect->caster.get().get();
			}
			this->hasDuration = this->HasDuration();
		}
	}

	bool Magic::HasDuration()  {
		if (this->activeEffect) {
			auto spell = this->activeEffect->spell;
			if (spell) {
				// switch (this->activeEffect->spell->GetSpellType()) {
				// 	case  MagicSystem::SpellType::kEnchantment: {
				// 		return false;
				// 	}
				// }
				switch (spell->GetCastingType()) {
					case  MagicSystem::CastingType::kConstantEffect: {
						return false;
					}
				}
			}
		}
		auto effectSetting = this->effectSetting;
		if (effectSetting) {
			if (effectSetting->data.flags.all(EffectSetting::EffectSettingData::Flag::kNoDuration)) {
				return false;
			}
		}
		return true;
	}

	void Magic::poll() {
		switch (this->state) {
			case State::Init:
			{
				this->dual_casted = this->IsDualCasting();

				this->state = State::Start;
				break;
			}
			case State::Start: {
				this->OnStart();
				this->state = State::Update;
				break;
			}
			case State::Update: {
				if (this->activeEffect->flags & ActiveEffect::Flag::kInactive) {
					break;
				}
				this->OnUpdate();
				if ((this->activeEffect->flags & ActiveEffect::Flag::kDispelled)
				    || (this->hasDuration && (this->activeEffect->elapsedSeconds >= this->activeEffect->duration))) {
					this->state = State::Finish;
				}
				break;
			}
			case State::Finish: {
				this->OnFinish();
				this->state = State::CleanUp;
				break;
			}
			case State::CleanUp: {
				break;
			}
		}
	}

	Actor* Magic::GetTarget() {
		return this->target;
	}
	Actor* Magic::GetCaster() {
		return this->caster;
	}

	ActiveEffect* Magic::GetActiveEffect() {
		return this->activeEffect;
	}

	EffectSetting* Magic::GetBaseEffect() {
		return this->effectSetting;
	}

	void Magic::Dispel() {
		if (this->activeEffect) {
			this->activeEffect->Dispel(false); // Not forced
		}
	}

	bool Magic::IsDualCasting() {
		if (this->caster) {
			auto casting_type = GetActiveEffect()->castingSource;
			if (casting_type == MagicSystem::CastingSource::kLeftHand || casting_type == MagicSystem::CastingSource::kRightHand) {
				auto source = this->caster->GetMagicCaster(casting_type);
				if (source) {
					return source->GetIsDualCasting();
				}
			}
		}
		return false;
	}

	MagicManager& MagicManager::GetSingleton() noexcept {
		static MagicManager instance;
		return instance;
	}

	void MagicManager::ProcessActiveEffects(Actor* actor) {
		auto& runtime = Runtime::GetSingleton();

		auto effect_list =actor->GetActiveEffectList();
		if (!effect_list) {
			return;
		}
		for (auto effect: (*effect_list)) {
			EffectSetting* base_spell = effect->GetBaseObject();
			if (ExplosiveGrowth::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new ExplosiveGrowth(effect));
			}

			if (Gigantism::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new Gigantism(effect));
			}

			if (GrowthPotion::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new GrowthPotion(effect));
			}

			if (SizePotion::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new SizePotion(effect));
			}

			if (SizeHunger::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new SizeHunger(effect));
			}

			if (CrushGrowth::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new CrushGrowth(effect));
			}

			if (TrackSize::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new TrackSize(effect));
			}

			if (ShrinkFoe::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new ShrinkFoe(effect));
			}

			if (SwordOfSize::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new SwordOfSize(effect));
			}

			if (ShrinkButton::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new ShrinkButton(effect));
			}
			if (GrowButton::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new GrowButton(effect));
			}

			if (SlowGrow::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new SlowGrow(effect));
			}

			if (SmallMassiveThreat::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new SmallMassiveThreat(effect));
			}

			if (Growth::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new Growth(effect));
			}
			if (Shrink::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new Shrink(effect));
			}

			if (GrowOther::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new GrowOther(effect));
			}
			if (ShrinkOther::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new ShrinkOther(effect));
			}

			if (GrowOtherButton::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new GrowOtherButton(effect));
			}
			if (ShrinkOtherButton::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new ShrinkOtherButton(effect));
			}

			if (ShrinkBack::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new ShrinkBack(effect));
			}
			if (ShrinkBackOther::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new ShrinkBackOther(effect));
			}

			if (VoreGrowth::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new VoreGrowth(effect));
			}
			if (SizeDamage::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new SizeDamage(effect));
			}
			if (Absorb::StartEffect(base_spell)) {
				this->active_effects.try_emplace(effect, new Absorb(effect));
			}
		}
	}

	void MagicManager::Update() {
		for (auto actor: find_actors()) {
			this->ProcessActiveEffects(actor);
		}

		for (auto i = this->active_effects.begin(); i != this->active_effects.end();) {
			auto& magic = (*i);
			magic.second->poll();
			if (magic.second->IsFinished()) {
				i = this->active_effects.erase(i);
			} else {
				++i;
			}
		}
	}

	void MagicManager::Reset() {
		this->active_effects.clear();
	}
}
