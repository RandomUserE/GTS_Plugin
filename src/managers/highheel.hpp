#pragma once
#include "data/transient.hpp"
#include "events.hpp"
// Module that handles high heel

using namespace RE;

namespace Gts {
	class HighHeelManager : public EventListener {
		public:
			[[nodiscard]] static HighHeelManager& GetSingleton() noexcept;

			virtual void PapyrusUpdate() override;
			virtual void ActorEquip(Actor* actor) override;
			virtual void ActorLoaded(Actor* actor) override;

			void ApplyHH(Actor* actor, bool force);
	};
}
