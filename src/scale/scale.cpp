#include "managers/GtsManager.hpp"
#include "managers/GtsSizeManager.hpp"
#include "data/persistent.hpp"
#include "data/runtime.hpp"
#include "scale/height.hpp"
#include "util.hpp"

using namespace Gts;

namespace {
	const float EPS = 1e-4;
}

namespace Gts {
	void set_target_scale(Actor* actor, float scale) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				if (scale < (actor_data->max_scale + EPS)) {
					// If new value is below max: allow it
					scale = scale;
				} else if (actor_data->target_scale < (actor_data->max_scale - EPS)) {
					// If we are below max currently and we are trying to scale over max: make it max
					actor_data->target_scale = scale;
				} else {
					// If we are over max: forbid it
				}
			}
		}
	}

	float get_target_scale(Actor* actor) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				return actor_data->target_scale;
			}
		}
		return -1.0;
	}

	void mod_target_scale(Actor* actor, float amt) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			auto runtime = Runtime::GetSingleton();
			if (SizeManager::GetSingleton().BalancedMode() >= 2.0 && amt > 0 && actor->formID == 0x14 || actor->IsPlayerTeammate() || actor->IsInFaction(runtime.FollowerFaction)) 
			{
				float scale = actor_data->visual_scale; // Enabled if BalanceMode is True. Decreases Grow Efficiency.
				if (scale >= 1.0)
				{
					amt /= (1.5 + (scale/1.5));
				}
			}
			if (actor->HasPerk(runtime.OnTheEdge))
			{
				float GetHP = clamp(0.5, 1.0, GetHealthPercentage(actor) + 0.4); // Bonus Size Gain if Actor has perk
				if (amt > 0) {
					amt /= GetHP;
				}
				else if (amt < 0) {
					amt *= GetHP;
				}
			}
			if (actor_data) {
				if (amt - EPS < 0.0) {
					// If neative change always: allow
					actor_data->target_scale += amt;
				} else if (actor_data->target_scale + amt < (actor_data->max_scale + EPS)) {
					// If change results is below max: allow it
					actor_data->target_scale += amt;
				} else if (actor_data->target_scale < (actor_data->max_scale - EPS)) {
					// If we are currently below max and we are scaling above max: make it max
					actor_data->target_scale = actor_data->max_scale;
				} else {
					// if we are over max then don't allow it
				}
			}
		}
	}

	void set_max_scale(Actor* actor, float scale) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				actor_data->max_scale = scale;
			}
		}
	}

	float get_max_scale(Actor* actor) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				return actor_data->max_scale;
			}
		}
		return -1.0;
	}
	void mod_max_scale(Actor* actor, float amt) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				actor_data->max_scale += amt;
			}
		}
	}

	float get_visual_scale(Actor* actor) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				return actor_data->visual_scale;
			}
		}
		return -1.0;
	}

	float get_natural_scale(Actor* actor) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				return actor_data->native_scale;
			}
		}
		return -1.0;
	}

	float get_effective_scale(Actor* actor) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				return actor_data->visual_scale * actor_data->effective_multi; // Effective Multi = SmallMassiveThreat
			}
		}
		return -1.0;
	}
}
