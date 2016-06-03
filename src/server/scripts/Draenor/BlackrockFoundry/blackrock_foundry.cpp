////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

# include "boss_operator_thogar.hpp"

/// Iron Flame Binder - 87515
class npc_foundry_iron_flame_binder : public CreatureScript
{
    public:
        npc_foundry_iron_flame_binder() : CreatureScript("npc_foundry_iron_flame_binder") { }

        enum eSpells
        {
            FireChannelCosmetic = 174883,

            Cauterize           = 175620,
            Lavaburst           = 175621,
            UnbindFlame         = 175609
        };

        enum eEvents
        {
            EventCauterize = 1,
            EventLavaburst,
            EventUnbindFlame
        };

        enum eCreature
        {
            FoundryArtifact = 87516
        };

        struct npc_foundry_iron_flame_binderAI : public ScriptedAI
        {
            npc_foundry_iron_flame_binderAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                AddTimedDelayedOperation(urand(50, 350), [this]() -> void
                {
                    std::list<Creature*> l_Artifacts;
                    me->GetCreatureListWithEntryInGrid(l_Artifacts, eCreature::FoundryArtifact, 30.0f);

                    for (Creature* l_Artifact : l_Artifacts)
                    {
                        if (l_Artifact->HasAura(eSpells::FireChannelCosmetic))
                            continue;

                        me->CastSpell(l_Artifact, eSpells::FireChannelCosmetic, true);
                    }
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventCauterize, 6 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventLavaburst, 2 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventUnbindFlame, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventCauterize:
                    {
                        if (Unit* l_Ally = me->SelectNearbyMostInjuredAlly())
                            me->CastSpell(l_Ally, eSpells::Cauterize, false);

                        m_Events.ScheduleEvent(eEvents::EventCauterize, 8 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventLavaburst:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::Lavaburst, false);

                        m_Events.ScheduleEvent(eEvents::EventLavaburst, 4 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventUnbindFlame:
                    {
                        me->CastSpell(me, eSpells::UnbindFlame, false);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_flame_binderAI(p_Creature);
        }
};

/// Living Flames - 87993
class npc_foundry_living_flames : public CreatureScript
{
    public:
        npc_foundry_living_flames() : CreatureScript("npc_foundry_living_flames") { }

        enum eSpell
        {
            UnleashFlame = 175613
        };

        struct npc_foundry_living_flamesAI : public ScriptedAI
        {
            npc_foundry_living_flamesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->CastSpell(me, eSpell::UnleashFlame, false);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                ScriptedAI::UpdateAI(p_Diff);

                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_living_flamesAI(p_Creature);
        }
};

/// Workshop Guardian - 87411
class npc_foundry_workshop_guardian : public CreatureScript
{
    public:
        npc_foundry_workshop_guardian() : CreatureScript("npc_foundry_workshop_guardian") { }

        enum eSpells
        {
            GrievousMortalWounds    = 175624,
            SpinningBladeMissile    = 175627
        };

        enum eEvents
        {
            EventGrievousMortalWounds = 1,
            EventSpinningBlade
        };

        struct npc_foundry_workshop_guardianAI : public ScriptedAI
        {
            npc_foundry_workshop_guardianAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventGrievousMortalWounds, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventSpinningBlade, 8 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventGrievousMortalWounds:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::GrievousMortalWounds, false);

                        m_Events.ScheduleEvent(eEvents::EventGrievousMortalWounds, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSpinningBlade:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eSpells::SpinningBladeMissile, false);

                        m_Events.ScheduleEvent(eEvents::EventSpinningBlade, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_workshop_guardianAI(p_Creature);
        }
};

/// Spinning Blade - 88008
class npc_foundry_spinning_blade : public CreatureScript
{
    public:
        npc_foundry_spinning_blade() : CreatureScript("npc_foundry_spinning_blade") { }

        enum eSpell
        {
            SpinningBladeAura = 175644
        };

        struct npc_foundry_spinning_bladeAI : public ScriptedAI
        {
            npc_foundry_spinning_bladeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->CastSpell(me, eSpell::SpinningBladeAura, false);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                me->DespawnOrUnsummon(20 * TimeConstants::IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_spinning_bladeAI(p_Creature);
        }
};

/// Ogron Hauler - 87719
class npc_foundry_ogron_hauler : public CreatureScript
{
    public:
        npc_foundry_ogron_hauler() : CreatureScript("npc_foundry_ogron_hauler") { }

        enum eSpells
        {
            ThrowSlagContainmentCrate       = 175290,
            CarryingSlagContainmentCrate    = 175285,

            GroundStomp                     = 175762,
            OverheadSmash                   = 175765,
            SlagBreath                      = 175752
        };

        enum eEvents
        {
            EventGroundStomp = 1,
            EventOverheadSmash,
            EventSlagBreath
        };

        struct npc_foundry_ogron_haulerAI : public ScriptedAI
        {
            npc_foundry_ogron_haulerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                me->CastSpell(me ,eSpells::CarryingSlagContainmentCrate, true);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->RemoveAura(eSpells::CarryingSlagContainmentCrate);

                me->CastSpell(p_Attacker, eSpells::ThrowSlagContainmentCrate, true);

                m_Events.ScheduleEvent(eEvents::EventGroundStomp, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventOverheadSmash, 8 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventSlagBreath, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventGroundStomp:
                    {
                        me->CastSpell(me, eSpells::GroundStomp, false);
                        m_Events.ScheduleEvent(eEvents::EventGroundStomp, 12 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventOverheadSmash:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::OverheadSmash, false);

                        m_Events.ScheduleEvent(eEvents::EventOverheadSmash, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSlagBreath:
                    {
                        me->CastSpell(me, eSpells::SlagBreath, false);
                        m_Events.ScheduleEvent(eEvents::EventSlagBreath, 12 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_ogron_haulerAI(p_Creature);
        }
};

/// Ironworker - 87448
class npc_foundry_ironworker : public CreatureScript
{
    public:
        npc_foundry_ironworker() : CreatureScript("npc_foundry_ironworker") { }

        enum eSpells
        {
            EmptySack           = 175671,
            PackingExplosiveOre = 175670,
            ThrowExplosiveOre   = 175672,
            HammerThrow         = 175668
        };

        enum eEvents
        {
            EventThrowExplosiveOre = 1,
            EventHammerThrow
        };

        struct npc_foundry_ironworkerAI : public ScriptedAI
        {
            npc_foundry_ironworkerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                if (!me->HasAura(eSpells::PackingExplosiveOre))
                    me->CastSpell(me, eSpells::EmptySack, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_ANIM_TIER, 0);

                if (me->HasAura(eSpells::PackingExplosiveOre))
                    m_Events.ScheduleEvent(eEvents::EventThrowExplosiveOre, 4 * TimeConstants::IN_MILLISECONDS);
                else
                    m_Events.ScheduleEvent(eEvents::EventHammerThrow, 4 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventThrowExplosiveOre:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eSpells::ThrowExplosiveOre, true);

                        m_Events.ScheduleEvent(eEvents::EventThrowExplosiveOre, 8 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHammerThrow:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eSpells::HammerThrow, false);

                        m_Events.ScheduleEvent(eEvents::EventHammerThrow, 8 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_ironworkerAI(p_Creature);
        }
};

/// Flame Jets - 87843
class npc_foundry_flame_jets : public CreatureScript
{
    public:
        npc_foundry_flame_jets() : CreatureScript("npc_foundry_flame_jets") { }

        enum eSpell
        {
            FlameOn = 175459
        };

        struct npc_foundry_flame_jetsAI : public ScriptedAI
        {
            npc_foundry_flame_jetsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_CastTimer = urand(2 * TimeConstants::IN_MILLISECONDS, 8 * TimeConstants::IN_MILLISECONDS);
            }

            uint32 m_CastTimer;

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_CastTimer)
                {
                    if (m_CastTimer <= p_Diff)
                    {
                        me->CastSpell(me, eSpell::FlameOn, true);
                        m_CastTimer = urand(2 * TimeConstants::IN_MILLISECONDS, 8 * TimeConstants::IN_MILLISECONDS);
                    }
                    else
                        m_CastTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_flame_jetsAI(p_Creature);
        }
};

/// Iron Slag-Shaper - 87521
class npc_foundry_iron_slag_shaper : public CreatureScript
{
    public:
        npc_foundry_iron_slag_shaper() : CreatureScript("npc_foundry_iron_slag_shaper") { }

        enum eSpells
        {
            AnimateSlag         = 175088,
            GrippingSlag        = 175603,
            RainOfSlag          = 175981,
            RainOfSlagVisual    = 175983
        };

        enum eEvents
        {
            EventGrippingSlag = 1,
            EventRainOfSlag
        };

        enum eAction
        {
            ActionRescheduleSlag
        };

        struct npc_foundry_iron_slag_shaperAI : public ScriptedAI
        {
            npc_foundry_iron_slag_shaperAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->CastSpell(me, eSpells::AnimateSlag, false);
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventGrippingSlag, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventRainOfSlag, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionRescheduleSlag)
                {
                    AddTimedDelayedOperation(100, [this]() -> void
                    {
                        me->CastSpell(me, eSpells::AnimateSlag, false);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventGrippingSlag:
                    {
                        me->CastSpell(me, eSpells::GrippingSlag, false);
                        m_Events.ScheduleEvent(eEvents::EventGrippingSlag, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRainOfSlag:
                    {
                        me->CastSpell(me, eSpells::RainOfSlag, false);
                        me->CastSpell(me, eSpells::RainOfSlagVisual, true);
                        m_Events.ScheduleEvent(eEvents::EventRainOfSlag, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_slag_shaperAI(p_Creature);
        }
};

/// Slagshop Worker - 87771
class npc_foundry_slagshop_worker : public CreatureScript
{
    public:
        npc_foundry_slagshop_worker() : CreatureScript("npc_foundry_slagshop_worker") { }

        enum eSpells
        {
            PunctureWound   = 175987,
            ThrowCoin       = 177475
        };

        enum eEvent
        {
            EventPunctureWound = 1
        };

        enum eTalk
        {
            TalkCosmetic
        };

        enum eCreature
        {
            SlagshopBrute = 87780
        };

        struct npc_foundry_slagshop_workerAI : public ScriptedAI
        {
            npc_foundry_slagshop_workerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                if (me->FindNearestCreature(eCreature::SlagshopBrute, 10.0f) != nullptr)
                {
                    AddTimedDelayedOperation(urand(1 * TimeConstants::IN_MILLISECONDS, 120 * TimeConstants::IN_MILLISECONDS), [this]() -> void
                    {
                        Talk(eTalk::TalkCosmetic);

                        me->CastSpell(me, eSpells::ThrowCoin, true);
                    });
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                ClearDelayedOperations();

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_ANIM_TIER, 0);

                m_Events.ScheduleEvent(eEvent::EventPunctureWound, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventPunctureWound:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::PunctureWound, true);

                        m_Events.ScheduleEvent(eEvent::EventPunctureWound, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void LastOperationCalled() override
            {
                AddTimedDelayedOperation(urand(1 * TimeConstants::IN_MILLISECONDS, 120 * TimeConstants::IN_MILLISECONDS), [this]() -> void
                {
                    Talk(eTalk::TalkCosmetic);

                    me->CastSpell(me, eSpells::ThrowCoin, true);
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_slagshop_workerAI(p_Creature);
        }
};

/// Slagshop Brute - 87780
class npc_foundry_slagshop_brute : public CreatureScript
{
    public:
        npc_foundry_slagshop_brute() : CreatureScript("npc_foundry_slagshop_brute") { }

        enum eSpells
        {
            LumberingStrength   = 175993,
            DeathHowl           = 175992
        };

        enum eEvents
        {
            EventDeathHowl = 1,
            EventLumberingStrength
        };

        struct npc_foundry_slagshop_bruteAI : public ScriptedAI
        {
            npc_foundry_slagshop_bruteAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->SetAIAnimKitId(0);

                m_Events.ScheduleEvent(eEvents::EventDeathHowl, 8 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventLumberingStrength, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventDeathHowl:
                    {
                        me->CastSpell(me, eSpells::DeathHowl, false);
                        m_Events.ScheduleEvent(eEvents::EventDeathHowl, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventLumberingStrength:
                    {
                        me->CastSpell(me, eSpells::LumberingStrength, false);
                        m_Events.ScheduleEvent(eEvents::EventLumberingStrength, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_slagshop_bruteAI(p_Creature);
        }
};

/// Iron Journeyman - 80677
class npc_foundry_iron_journeyman : public CreatureScript
{
    public:
        npc_foundry_iron_journeyman() : CreatureScript("npc_foundry_iron_journeyman") { }

        enum eSpell
        {
            Heymaker = 164102
        };

        enum eEvent
        {
            EventHeymaker = 1
        };

        struct npc_foundry_iron_journeymanAI : public ScriptedAI
        {
            npc_foundry_iron_journeymanAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_ANIM_TIER, 0);

                m_Events.ScheduleEvent(eEvent::EventHeymaker, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventHeymaker:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpell::Heymaker, false);

                        m_Events.ScheduleEvent(eEvent::EventHeymaker, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_journeymanAI(p_Creature);
        }
};

/// Gronnling Laborer - 81114
class npc_foundry_gronnling_laborer : public CreatureScript
{
    public:
        npc_foundry_gronnling_laborer() : CreatureScript("npc_foundry_gronnling_laborer") { }

        enum eSpells
        {
            Enrage          = 18501,
            GronnlingSmash  = 169402,
            EarthAttack     = 165318,
            HeadSmashDmg    = 170604,
            HeadSmashDust   = 165427
        };

        enum eEvents
        {
            EventGronnlingSmash = 1,
            EventHeadSmash
        };

        struct npc_foundry_gronnling_laborerAI : public ScriptedAI
        {
            npc_foundry_gronnling_laborerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_ANIM_TIER, 0);

                me->RemoveAura(eSpells::EarthAttack);

                m_Events.ScheduleEvent(eEvents::EventGronnlingSmash, 10 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventHeadSmash, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HasAura(eSpells::Enrage))
                    return;

                if (me->HealthBelowPctDamaged(50, p_Damage))
                    me->CastSpell(me, eSpells::Enrage, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventGronnlingSmash:
                    {
                        me->CastSpell(me, eSpells::GronnlingSmash, false);
                        m_Events.ScheduleEvent(eEvents::EventGronnlingSmash, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHeadSmash:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                        {
                            me->CastSpell(l_Target, eSpells::HeadSmashDmg, true);
                            me->CastSpell(me, eSpells::HeadSmashDust, true);
                        }

                        m_Events.ScheduleEvent(eEvents::EventHeadSmash, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_gronnling_laborerAI(p_Creature);
        }
};

/// Karnor the Cruel - 81117
class npc_foundry_karnor_the_cruel : public CreatureScript
{
    public:
        npc_foundry_karnor_the_cruel() : CreatureScript("npc_foundry_karnor_the_cruel") { }

        enum eSpells
        {
            ChainVolley             = 170679,
            ChainVolleyDummy        = 170680,
            KillingSpree            = 170687,
            EnvenomedBladeTrigger   = 177943,
            EnvenomedBlade          = 170689
        };

        enum eEvents
        {
            EventChainVolley = 1,
            EventKillingSpree
        };

        enum eData
        {
            ChainVolleyCount = 7
        };

        struct npc_foundry_karnor_the_cruelAI : public ScriptedAI
        {
            npc_foundry_karnor_the_cruelAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventChainVolley, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventKillingSpree, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::ChainVolley:
                    {
                        for (uint8 l_I = 0; l_I < eData::ChainVolleyCount; ++l_I)
                            p_Target->CastSpell(p_Target, eSpells::ChainVolleyDummy, true);

                        break;
                    }
                    case eSpells::EnvenomedBladeTrigger:
                    {
                        me->NearTeleportTo(*p_Target);

                        uint64 l_Guid = p_Target->GetGUID();
                        AddTimedDelayedOperation(100, [this, l_Guid]() -> void
                        {
                            if (Unit* l_Target = Unit::GetUnit(*me, l_Guid))
                                me->CastSpell(l_Target, eSpells::EnvenomedBlade, false);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventChainVolley:
                    {
                        me->CastSpell(me, eSpells::ChainVolley, true);
                        m_Events.ScheduleEvent(eEvents::EventChainVolley, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventKillingSpree:
                    {
                        me->CastSpell(me, eSpells::KillingSpree, true);
                        m_Events.ScheduleEvent(eEvents::EventKillingSpree, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_karnor_the_cruelAI(p_Creature);
        }
};

/// Iron Laborer (Cosmetic) - 78977
class npc_foundry_iron_laborer_cosmetic : public CreatureScript
{
    public:
        npc_foundry_iron_laborer_cosmetic() : CreatureScript("npc_foundry_iron_laborer_cosmetic") { }

        enum eActions
        {
            ActionDisable   = 0,
            ActionEvent     = 0
        };

        enum eCreature
        {
            IronLaborer = 78917
        };

        struct npc_foundry_iron_laborer_cosmeticAI : public ScriptedAI
        {
            npc_foundry_iron_laborer_cosmeticAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Started(false) { }

            bool m_Started;

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActions::ActionDisable)
                    m_Started = true;
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (m_Started)
                    return;

                if (p_Who->GetTypeId() != TypeID::TYPEID_PLAYER)
                    return;

                if (p_Who->GetDistance(me) > 30.0f)
                    return;

                m_Started = true;

                std::list<Creature*> l_Creatures;
                me->GetCreatureListWithEntryInGrid(l_Creatures, me->GetEntry(), 200.0f);

                l_Creatures.remove(me);

                for (Creature* l_Me : l_Creatures)
                {
                    if (l_Me->IsAIEnabled)
                        l_Me->AI()->DoAction(eActions::ActionDisable);
                }

                LaunchTunnelEvent();
            }

            void LaunchTunnelEvent()
            {
                std::list<Creature*> l_Creatures;
                me->GetCreatureListWithEntryInGrid(l_Creatures, eCreature::IronLaborer, 200.0f);

                for (Creature* l_Laborer : l_Creatures)
                {
                    if (l_Laborer->IsAIEnabled)
                        l_Laborer->AI()->DoAction(eActions::ActionEvent);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_laborer_cosmeticAI(p_Creature);
        }
};

/// Iron Laborer - 78917
class npc_foundry_iron_laborer : public CreatureScript
{
    public:
        npc_foundry_iron_laborer() : CreatureScript("npc_foundry_iron_laborer") { }

        enum eAction
        {
            ActionEvent
        };

        enum eCreatures
        {
            DarkshardAcidback   = 78801,
            DarkshardGnasher    = 78978
        };

        enum eMoves
        {
            MoveFirst,
            MoveSecond
        };

        struct npc_foundry_iron_laborerAI : public ScriptedAI
        {
            npc_foundry_iron_laborerAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Vehicle(p_Creature->GetVehicleKit()) { }

            Vehicle* m_Vehicle;

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionEvent)
                {
                    std::list<Creature*> l_Darkshards;
                    me->GetCreatureListWithEntryInGrid(l_Darkshards, eCreatures::DarkshardAcidback, 5.0f);

                    uint8 l_DarkshardCount = 2;
                    for (Creature* l_Darkshard : l_Darkshards)
                    {
                        if (l_Darkshard->GetVehicle())
                            continue;

                        l_Darkshard->EnterVehicle(me);

                        if (!--l_DarkshardCount)
                            break;
                    }

                    l_Darkshards.clear();

                    me->GetCreatureListWithEntryInGrid(l_Darkshards, eCreatures::DarkshardGnasher, 20.0f);

                    for (Creature* l_Darkshard : l_Darkshards)
                    {
                        if (l_Darkshard->IsAIEnabled)
                            l_Darkshard->AI()->DoAction(eAction::ActionEvent);
                    }

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        float l_X, l_Y, l_Z, l_O;

                        l_O = me->GetOrientation();
                        l_X = me->GetPositionX() + (30.0f * cos(l_O));
                        l_Y = me->GetPositionY() + (30.0f * sin(l_O));

                        l_Z = me->GetPositionZ();
                        l_Z = me->GetMap()->GetHeight(me->GetPhaseMask(), l_X, l_Y, l_Z) + 1.0f;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(eMoves::MoveFirst, l_X, l_Y, l_Z);
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoves::MoveFirst:
                    {
                        AddTimedDelayedOperation(200, [this]() -> void
                        {
                            float l_X, l_Y, l_Z;

                            l_X = me->GetPositionX() + (30.0f * cos(0.0f));
                            l_Y = me->GetPositionY() + (30.0f * sin(0.0f));

                            l_Z = me->GetPositionZ();
                            l_Z = me->GetMap()->GetHeight(me->GetPhaseMask(), l_X, l_Y, l_Z);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveSecond, l_X, l_Y, l_Z);
                        });

                        break;
                    }
                    case eMoves::MoveSecond:
                    {
                        m_Vehicle->RemoveAllPassengers();
                        me->Kill(me);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_laborerAI(p_Creature);
        }
};

/// Darkshard Acidback - 78801
class npc_foundry_darkshard_acidback : public CreatureScript
{
    public:
        npc_foundry_darkshard_acidback() : CreatureScript("npc_foundry_darkshard_acidback") { }

        struct npc_foundry_darkshard_acidbackAI : public ScriptedAI
        {
            npc_foundry_darkshard_acidbackAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void OnVehicleExited(Unit* /*p_Vehicle*/) override
            {
                me->SetHomePosition(*me);

                if (Player* l_Target = me->SelectNearestPlayerNotGM(15.0f))
                    AttackStart(l_Target);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_darkshard_acidbackAI(p_Creature);
        }
};

/// Darkshard Gnasher - 78978
class npc_foundry_darkshard_gnasher : public CreatureScript
{
    public:
        npc_foundry_darkshard_gnasher() : CreatureScript("npc_foundry_darkshard_gnasher") { }

        enum eAction
        {
            ActionEvent
        };

        enum eMoves
        {
            MoveFirst,
            MoveSecond
        };

        enum eSpells
        {
            RollingMovement         = 159511,

            ShatteringChargeSeacher = 159551,
            ShatteringChargeTrigger = 159518,
            ShatteringChargeEnding  = 159519,

            InsatiableHungerSearch  = 159631,
            InsatiableHungerTrigger = 159632,
            InsatiableHungerAura    = 159700
        };

        enum eEvents
        {
            EventShatteringCharge = 1,
            EventInsatiableHunger
        };

        struct npc_foundry_darkshard_gnasherAI : public ScriptedAI
        {
            npc_foundry_darkshard_gnasherAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Moved(false) { }

            EventMap m_Events;

            bool m_Moved;

            uint64 m_FixateTarget;

            void Reset() override
            {
                m_Events.Reset();

                m_FixateTarget = 0;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventShatteringCharge, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventInsatiableHunger, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionEvent && !m_Moved)
                {
                    m_Moved = true;

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        me->CastSpell(me, eSpells::RollingMovement, true);

                        float l_X, l_Y, l_Z, l_O;

                        l_O = me->GetOrientation();
                        l_X = me->GetPositionX() + (35.0f * cos(l_O));
                        l_Y = me->GetPositionY() + (35.0f * sin(l_O));

                        l_Z = me->GetPositionZ();
                        l_Z = me->GetMap()->GetHeight(me->GetPhaseMask(), l_X, l_Y, l_Z);

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(eMoves::MoveFirst, l_X, l_Y, l_Z);
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoves::MoveFirst:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            float l_X, l_Y, l_Z;

                            l_X = me->GetPositionX() + (30.0f * cos(0.0f));
                            l_Y = me->GetPositionY() + (30.0f * sin(0.0f));

                            l_Z = me->GetPositionZ();
                            l_Z = me->GetMap()->GetHeight(me->GetPhaseMask(), l_X, l_Y, l_Z);

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(eMoves::MoveSecond, l_X, l_Y, l_Z);
                        });

                        break;
                    }
                    case eMoves::MoveSecond:
                    {
                        me->RemoveAura(eSpells::RollingMovement);

                        if (Player* l_Target = me->SelectNearestPlayerNotGM(15.0f))
                            AttackStart(l_Target);

                        break;
                    }
                    case EventId::EVENT_CHARGE:
                    {
                        me->CastSpell(me, eSpells::ShatteringChargeEnding, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::ShatteringChargeSeacher:
                    {
                        me->CastSpell(p_Target, eSpells::ShatteringChargeTrigger, false);
                        break;
                    }
                    case eSpells::InsatiableHungerSearch:
                    {
                        m_FixateTarget = p_Target->GetGUID();
                        me->CastSpell(p_Target, eSpells::InsatiableHungerTrigger, false);

                        AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void { m_FixateTarget = 0; });

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_FixateTarget)
                {
                    if (Unit* l_Target = Unit::GetUnit(*me, m_FixateTarget))
                    {
                        if (l_Target->isAlive())
                        {
                            AttackStart(l_Target);
                            DoMeleeAttackIfReady();
                            return;
                        }
                        else
                        {
                            me->RemoveAura(eSpells::InsatiableHungerAura);
                            m_FixateTarget = 0;
                        }
                    }
                }

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventShatteringCharge:
                    {
                        me->CastSpell(me, eSpells::ShatteringChargeSeacher, true);
                        m_Events.ScheduleEvent(eEvents::EventShatteringCharge, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventInsatiableHunger:
                    {
                        me->CastSpell(me, eSpells::InsatiableHungerSearch, true);
                        m_Events.ScheduleEvent(eEvents::EventInsatiableHunger, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_darkshard_gnasherAI(p_Creature);
        }
};

/// Darkshard Crystalback - 78233
class npc_foundry_darkshard_crystalback : public CreatureScript
{
    public:
        npc_foundry_darkshard_crystalback() : CreatureScript("npc_foundry_darkshard_crystalback") { }

        enum eSpells
        {
            Acidmaw             = 159939,
            ShardVolleySearcher = 159769,
            ShardVolleyTrigger  = 159775
        };

        enum eEvents
        {
            EventAcidMaw = 1,
            EventShardVolley
        };

        struct npc_foundry_darkshard_crystalbackAI : public ScriptedAI
        {
            npc_foundry_darkshard_crystalbackAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventAcidMaw, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventShardVolley, 9 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::ShardVolleySearcher)
                {
                    me->CastSpell(p_Target, eSpells::ShardVolleyTrigger, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventAcidMaw:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::Acidmaw, true);

                        m_Events.ScheduleEvent(eEvents::EventAcidMaw, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventShardVolley:
                    {
                        me->CastSpell(me, eSpells::ShardVolleySearcher, false);
                        m_Events.ScheduleEvent(eEvents::EventShardVolley, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_darkshard_crystalbackAI(p_Creature);
        }
};

/// Slag Behemoth - 77504
class npc_foundry_slag_behemoth : public CreatureScript
{
    public:
        npc_foundry_slag_behemoth() : CreatureScript("npc_foundry_slag_behemoth") { }

        enum eSpells
        {
            IgniteSearcher      = 156345,

            VolcanicBombSearch  = 156348,
            VolcanicBombMissile = 156413,

            BlastWave           = 156446
        };

        enum eEvents
        {
            EventIgnite = 1,
            EventVolcanicBomb,
            EventBlastWave
        };

        enum eTalk
        {
            TalkBlastWave
        };

        enum eGameObjects
        {
            VolcanicBomb    = 227616,
            IceBlock        = 201722
        };

        enum eAction
        {
            ActionIntro
        };

        struct npc_foundry_slag_behemothAI : public ScriptedAI
        {
            npc_foundry_slag_behemothAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            uint64 m_EncounterDoor;

            void Reset() override
            {
                m_EncounterDoor = 0;

                m_Events.Reset();

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (GameObject* l_FurnaceDoor = me->FindNearestGameObject(eFoundryGameObjects::BlastFurnaceEncounterDoor, 30.0f))
                    {
                        m_EncounterDoor = l_FurnaceDoor->GetGUID();
                        l_FurnaceDoor->SetGoState(GOState::GO_STATE_READY);
                    }
                });

                std::list<GameObject*> l_Gobs;
                me->GetGameObjectListWithEntryInGrid(l_Gobs, eGameObjects::IceBlock, 100.0f);

                for (GameObject* l_GameObject : l_Gobs)
                    l_GameObject->Delete();

                l_Gobs.clear();

                me->GetGameObjectListWithEntryInGrid(l_Gobs, eGameObjects::VolcanicBomb, 100.0f);

                for (GameObject* l_GameObject : l_Gobs)
                    l_GameObject->Delete();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventIgnite, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventVolcanicBomb, 11 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventBlastWave, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (GameObject* l_FurnaceDoor = GameObject::GetGameObject(*me, m_EncounterDoor))
                    l_FurnaceDoor->SetGoState(GOState::GO_STATE_ACTIVE);

                std::list<GameObject*> l_Gobs;
                me->GetGameObjectListWithEntryInGrid(l_Gobs, eGameObjects::IceBlock, 100.0f);

                for (GameObject* l_GameObject : l_Gobs)
                    l_GameObject->Delete();

                l_Gobs.clear();

                me->GetGameObjectListWithEntryInGrid(l_Gobs, eGameObjects::VolcanicBomb, 100.0f);

                for (GameObject* l_GameObject : l_Gobs)
                    l_GameObject->Delete();

                if (InstanceScript* l_InstanceScript = me->GetInstanceScript())
                {
                    if (Creature* l_Creature = Creature::GetCreature(*me, l_InstanceScript->GetData64(eFoundryCreatures::BlackhandCosmetic)))
                    {
                        if (l_Creature->IsAIEnabled)
                            l_Creature->AI()->DoAction(eAction::ActionIntro);
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::VolcanicBombSearch:
                    {
                        me->CastSpell(*p_Target, eSpells::VolcanicBombMissile, false);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventIgnite:
                    {
                        me->CastSpell(me, eSpells::IgniteSearcher, false);
                        m_Events.ScheduleEvent(eEvents::EventIgnite, 13 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventVolcanicBomb:
                    {
                        me->CastSpell(me, eSpells::VolcanicBombSearch, true);
                        m_Events.ScheduleEvent(eEvents::EventVolcanicBomb, 11 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBlastWave:
                    {
                        Talk(eTalk::TalkBlastWave);

                        me->CastSpell(me, eSpells::BlastWave, false);
                        m_Events.ScheduleEvent(eEvents::EventBlastWave, 35 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_slag_behemothAI(p_Creature);
        }
};

/// Blackrock Enforcer - 79208
class npc_foundry_blackrock_enforcer : public CreatureScript
{
    public:
        npc_foundry_blackrock_enforcer() : CreatureScript("npc_foundry_blackrock_enforcer") { }

        enum eSpells
        {
            ClobberingStrike    = 160102,
            FireBombMissile     = 160250,
            Intimidation        = 160109
        };

        enum eEvents
        {
            EventClobberingStrike = 1,
            EventFireBomb,
            EventIntimidation
        };

        struct npc_foundry_blackrock_enforcerAI : public ScriptedAI
        {
            npc_foundry_blackrock_enforcerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventClobberingStrike, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventFireBomb, 7 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventIntimidation, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Hansgar = Creature::GetCreature(*me, l_Instance->GetData64(eFoundryCreatures::BossHansgar)))
                    {
                        if (l_Hansgar->IsAIEnabled)
                            l_Hansgar->AI()->SetGUID(me->GetGUID(), 0);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventClobberingStrike:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::ClobberingStrike, false);
                        m_Events.ScheduleEvent(eEvents::EventClobberingStrike, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFireBomb:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eSpells::FireBombMissile, false);
                        m_Events.ScheduleEvent(eEvents::EventFireBomb, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventIntimidation:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(*l_Target, eSpells::Intimidation, false);
                        m_Events.ScheduleEvent(eEvents::EventIntimidation, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_blackrock_enforcerAI(p_Creature);
        }
};

/// Blackrock Forge Specialist - 79200
class npc_foundry_blackrock_forge_specialist : public CreatureScript
{
    public:
        npc_foundry_blackrock_forge_specialist() : CreatureScript("npc_foundry_blackrock_forge_specialist") { }

        enum eSpells
        {
            RendingSlash    = 160092,
            ShreddingSpear  = 160079
        };

        enum eEvents
        {
            EventRendingSlash = 1,
            EventShreddingSpear
        };

        struct npc_foundry_blackrock_forge_specialistAI : public ScriptedAI
        {
            npc_foundry_blackrock_forge_specialistAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventRendingSlash, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventShreddingSpear, 8 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Hansgar = Creature::GetCreature(*me, l_Instance->GetData64(eFoundryCreatures::BossHansgar)))
                    {
                        if (l_Hansgar->IsAIEnabled)
                            l_Hansgar->AI()->SetGUID(me->GetGUID(), 0);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventRendingSlash:
                    {
                        me->CastSpell(me, eSpells::RendingSlash, false);
                        m_Events.ScheduleEvent(eEvents::EventRendingSlash, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventShreddingSpear:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 5.0f))
                            me->CastSpell(l_Target, eSpells::ShreddingSpear, false);
                        m_Events.ScheduleEvent(eEvents::EventShreddingSpear, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_blackrock_forge_specialistAI(p_Creature);
        }
};

/// Iron Taskmaster - 80708
class npc_foundry_iron_taskmaster : public CreatureScript
{
    public:
        npc_foundry_iron_taskmaster() : CreatureScript("npc_foundry_iron_taskmaster") { }

        enum eSpells
        {
            Bonk                = 163126,
            HeldToTaskSearcher  = 163125,
            HeldToTask          = 163121
        };

        enum eEvents
        {
            EventBonk = 1,
            EventHeldToTask
        };

        enum eAction
        {
            ActionIntro
        };

        enum eTalks
        {
            Intro,
            Aggro
        };

        struct npc_foundry_iron_taskmasterAI : public ScriptedAI
        {
            npc_foundry_iron_taskmasterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (!l_Instance->GetData(eFoundryDatas::IronTaskmasterAggro))
                    {
                        l_Instance->SetData(eFoundryDatas::IronTaskmasterAggro, 1);

                        Talk(eTalks::Aggro);
                    }
                }

                m_Events.ScheduleEvent(eEvents::EventBonk, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventHeldToTask, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eAction::ActionIntro:
                    {
                        Talk(eTalks::Intro);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::HeldToTaskSearcher)
                    me->CastSpell(p_Target, eSpells::HeldToTask, false);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventBonk:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::Bonk, false);

                        m_Events.ScheduleEvent(eEvents::EventBonk, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHeldToTask:
                    {
                        me->CastSpell(me, eSpells::HeldToTaskSearcher, true);
                        m_Events.ScheduleEvent(eEvents::EventHeldToTask, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_taskmasterAI(p_Creature);
        }
};

/// Iron Flametwister - 80676
class npc_foundry_iron_flametwister : public CreatureScript
{
    public:
        npc_foundry_iron_flametwister() : CreatureScript("npc_foundry_iron_flametwister") { }

        enum eSpells
        {
            LavaShield  = 163192,
            LavaBlast   = 178177,
            /// Cosmetic
            ImbueWeapon = 163093
        };

        enum eEvent
        {
            EventLavaBlast = 1
        };

        enum eCreatures
        {
            EnchantedArmament   = 80683,
            EnchantedArmament2  = 80702
        };

        struct npc_foundry_iron_flametwisterAI : public ScriptedAI
        {
            npc_foundry_iron_flametwisterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            bool m_Exploded;

            void Reset() override
            {
                m_Events.Reset();

                m_Exploded = false;

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    for (uint32 l_Entry = eFoundryGameObjects::ConveyorBelt006; l_Entry <= eFoundryGameObjects::ConveyorBelt008; ++l_Entry)
                    {
                        if (GameObject* l_Belt = me->FindNearestGameObject(l_Entry, 13.0f))
                        {
                            if (!me->FindNearestCreature(eCreatures::EnchantedArmament, 20.0f) &&
                                !me->FindNearestCreature(eCreatures::EnchantedArmament2, 20.0f))
                                break;

                            me->CastSpell(me, eSpells::ImbueWeapon, true);
                            break;
                        }
                    }
                });
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                ClearDelayedOperations();

                m_Events.ScheduleEvent(eEvent::EventLavaBlast, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(20, p_Damage) && !m_Exploded)
                {
                    m_Exploded = true;

                    me->CastSpell(me, eSpells::LavaShield, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventLavaBlast:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eSpells::LavaBlast, false);

                        m_Events.ScheduleEvent(eEvent::EventLavaBlast, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void LastOperationCalled() override
            {
                AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    for (uint32 l_Entry = eFoundryGameObjects::ConveyorBelt006; l_Entry <= eFoundryGameObjects::ConveyorBelt008; ++l_Entry)
                    {
                        if (GameObject* l_Belt = me->FindNearestGameObject(l_Entry, 13.0f))
                        {
                            if (!me->FindNearestCreature(eCreatures::EnchantedArmament, 20.0f) &&
                                !me->FindNearestCreature(eCreatures::EnchantedArmament2, 20.0f))
                                break;

                            me->CastSpell(me, eSpells::ImbueWeapon, true);
                            break;
                        }
                    }
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_flametwisterAI(p_Creature);
        }
};

/// Iron Smith - 80719
class npc_foundry_iron_smith : public CreatureScript
{
    public:
        npc_foundry_iron_smith() : CreatureScript("npc_foundry_iron_smith") { }

        enum eSpell
        {
            CrushingSlam = 163182
        };

        enum eEvent
        {
            EventCrushingSlam = 1
        };

        struct npc_foundry_iron_smithAI : public ScriptedAI
        {
            npc_foundry_iron_smithAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvent::EventCrushingSlam, 10 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventCrushingSlam:
                    {
                        me->CastSpell(me, eSpell::CrushingSlam, false);
                        m_Events.ScheduleEvent(eEvent::EventCrushingSlam, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_smithAI(p_Creature);
        }
};

/// Enchanted Armament - 80683
class npc_foundry_enchanted_armament : public CreatureScript
{
    public:
        npc_foundry_enchanted_armament() : CreatureScript("npc_foundry_enchanted_armament") { }

        struct npc_foundry_enchanted_armamentAI : public ScriptedAI
        {
            npc_foundry_enchanted_armamentAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                EnchantedArmamanetSpawn = 163050,
                ImbueWeapon             = 163089,
                Imbued                  = 163095,
                EmpoweredArmamentAoE    = 162337
            };

            enum eMove
            {
                MoveToOtherSide = 1
            };

            enum eEvent
            {
                EventEmpoweredArmament = 1
            };

            EventMap m_Event;

            void Reset() override
            {
                me->CastSpell(me, eSpells::EnchantedArmamanetSpawn, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                AddTimedDelayedOperation(200, [this]() -> void
                {
                    me->SetSpeed(UnitMoveType::MOVE_FLIGHT, me->GetSpeedRate(UnitMoveType::MOVE_RUN));

                    float l_X, l_Y, l_Z;
                    float l_O = me->GetOrientation();

                    float l_Radius = me->FindNearestGameObject(eFoundryGameObjects::ConveyorBelt007, 40.0f) ? 68.0f : 73.0f;

                    l_X = me->GetPositionX() + (l_Radius * cos(l_O));
                    l_Y = me->GetPositionY() + (l_Radius * sin(l_O));
                    l_Z = me->GetPositionZ();

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(eMove::MoveToOtherSide, l_X, l_Y, l_Z);
                });
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->GetMotionMaster()->Clear();

                AttackStart(p_Attacker);

                m_Event.ScheduleEvent(eEvent::EventEmpoweredArmament, 4 * TimeConstants::IN_MILLISECONDS);

                me->DespawnOrUnsummon(20 * TimeConstants::IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID == eMove::MoveToOtherSide)
                    me->DespawnOrUnsummon();
            }

            void SpellHit(Unit* /*p_Attacker*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::ImbueWeapon)
                {
                    me->CastSpell(me, eSpells::Imbued, true);

                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Event.Update(p_Diff);

                if (m_Event.ExecuteEvent() == eEvent::EventEmpoweredArmament)
                {
                    me->CastSpell(me, eSpells::EmpoweredArmamentAoE, true);

                    m_Event.ScheduleEvent(eEvent::EventEmpoweredArmament, 4 * TimeConstants::IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_enchanted_armamentAI(p_Creature);
        }
};

/// Flame Vents - 80681
class npc_foundry_flame_vents : public CreatureScript
{
    public:
        npc_foundry_flame_vents() : CreatureScript("npc_foundry_flame_vents") { }

        struct npc_foundry_flame_ventsAI : public ScriptedAI
        {
            npc_foundry_flame_ventsAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Initialized(false) { }

            enum eSpell
            {
                FlameVentCosmetics = 163045
            };

            bool m_Initialized;

            std::set<uint64> m_AffectedPlayers;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                std::list<Creature*> l_FlameVents;
                me->GetCreatureListWithEntryInGrid(l_FlameVents, me->GetEntry(), 10.0f);

                for (Creature* l_Trigger : l_FlameVents)
                {
                    if (npc_foundry_flame_vents::npc_foundry_flame_ventsAI* l_AI = CAST_AI(npc_foundry_flame_vents::npc_foundry_flame_ventsAI, l_Trigger->GetAI()))
                    {
                        /// Don't use all triggers as cosmetic triggers
                        /// Only the first will be enough
                        if (l_AI->m_Initialized)
                            return;
                    }
                }

                if ((me->GetOrientation() >= 3.14f && me->GetOrientation() <= 3.20f) ||
                    (me->GetOrientation() >= 4.70f && me->GetOrientation() <= 4.71f) || me->GetOrientation() < 0.1f)
                {
                    m_Initialized = true;

                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Position l_SpawnPos = *me;

                        l_SpawnPos.m_positionZ += 0.5f;

                        me->CastSpell(l_SpawnPos, eSpell::FlameVentCosmetics, true);
                    });
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (npc_foundry_enchanted_armament::npc_foundry_enchanted_armamentAI* l_AI = CAST_AI(npc_foundry_enchanted_armament::npc_foundry_enchanted_armamentAI, p_Summon->GetAI()))
                {
                    float l_O = me->GetOrientation();
                    l_AI->AddTimedDelayedOperation(50, [l_AI, l_O]() -> void
                    {
                        l_AI->me->SetFacingTo(l_O);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_Initialized)
                    return;

                Position l_ForcedPos;
                float l_MaxDist = 70.0f;
                float l_O = 0.0f;

                bool l_Negative = true;

                if (GameObject* l_Belt = me->FindNearestGameObject(eFoundryGameObjects::ConveyorBelt006, 40.0f))
                    l_O = M_PI;
                else if (GameObject* l_Belt = me->FindNearestGameObject(eFoundryGameObjects::ConveyorBelt007, 40.0f))
                {
                    l_Negative = false;
                    l_O = 0.0f;
                }
                else if (GameObject* l_Belt = me->FindNearestGameObject(eFoundryGameObjects::ConveyorBelt008, 45.0f))
                    l_O = M_PI;
                else
                    return;

                l_ForcedPos.m_positionX = me->GetPositionX() + (l_MaxDist * cos(l_O));
                l_ForcedPos.m_positionY = me->GetPositionY() + (l_MaxDist * sin(l_O));
                l_ForcedPos.m_positionZ = me->GetPositionZ();
                l_ForcedPos.m_orientation = l_O;

                std::set<uint64> l_Targets;

                Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    if (Player* l_Player = l_Iter->getSource())
                    {
                        if (l_Player->GetDistance(me) >= l_MaxDist)
                            continue;

                        if (l_Player->GetPositionY() <= me->GetPositionY() + 4.0f &&
                            l_Player->GetPositionY() >= me->GetPositionY() - 4.0f)
                            l_Targets.insert(l_Player->GetGUID());
                    }
                }

                for (std::set<uint64>::iterator l_Iter = m_AffectedPlayers.begin(); l_Iter != m_AffectedPlayers.end();)
                {
                    if (l_Targets.find((*l_Iter)) == l_Targets.end())
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, (*l_Iter)))
                            l_Player->SendApplyMovementForce(me->GetGUID(), false, Position());

                        l_Iter = m_AffectedPlayers.erase(l_Iter);
                    }
                    else
                        ++l_Iter;
                }

                for (uint64 l_Guid : l_Targets)
                {
                    if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                    {
                        if (!l_Player->HasMovementForce(me->GetGUID()))
                        {
                            l_Player->SendApplyMovementForce(me->GetGUID(), true, l_ForcedPos, 1.84f, 0, G3D::Vector3(l_Negative ? -1.84f : 1.84f, 0.0f, 0.0f));

                            m_AffectedPlayers.insert(l_Guid);
                        }
                    }
                }
            }

            void LastOperationCalled() override
            {
                if ((me->GetOrientation() >= 3.14f && me->GetOrientation() <= 3.20f) ||
                    (me->GetOrientation() >= 4.70f && me->GetOrientation() <= 4.71f) || me->GetOrientation() < 0.1f)
                {
                    AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Position l_SpawnPos = *me;

                        l_SpawnPos.m_positionZ += 0.5f;

                        me->CastSpell(l_SpawnPos, eSpell::FlameVentCosmetics, true);
                    });
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_flame_ventsAI(p_Creature);
        }
};

/// Mol'dana Two-Blade - 88902
class npc_foundry_moldana_two_blade : public CreatureScript
{
    public:
        npc_foundry_moldana_two_blade() : CreatureScript("npc_foundry_moldana_two_blade") { }

        enum eSpells
        {
            AkathaCosmetics         = 175413,
            RisingFlameKick         = 177891,
            /// Ember in the Wind
            EmberInTheWindSearcher  = 177854,
            EmberInTheWindAura      = 177855,
            EmberInTheWindCast      = 177856,
            EmberInTheWindNext      = 177858,
            EmberInTheWindTeleport  = 177859,
            EmberInTheWindAoE       = 177860
        };

        enum eEvents
        {
            EventRisingFlameKick = 1,
            EventEmberInTheWind,
            EventNextEmberInTheWind
        };

        struct npc_foundry_moldana_two_bladeAI : public ScriptedAI
        {
            npc_foundry_moldana_two_bladeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                me->CastSpell(me, eSpells::AkathaCosmetics, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                m_Events.ScheduleEvent(eEvents::EventRisingFlameKick, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventEmberInTheWind, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::EmberInTheWindSearcher:
                    {
                        me->CastSpell(p_Target, eSpells::EmberInTheWindAura, true);
                        break;
                    }
                    case eSpells::EmberInTheWindNext:
                    {
                        me->CastSpell(p_Target, eSpells::EmberInTheWindTeleport, true);
                        me->CastSpell(me, eSpells::EmberInTheWindAoE, true);

                        me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                        m_Events.ScheduleEvent(eEvents::EventNextEmberInTheWind, 1 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventRisingFlameKick:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::RisingFlameKick, true);

                        m_Events.ScheduleEvent(eEvents::EventRisingFlameKick, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventEmberInTheWind:
                    {
                        me->CastSpell(me, eSpells::EmberInTheWindSearcher, true);
                        me->CastSpell(me, eSpells::EmberInTheWindCast, false);
                        m_Events.ScheduleEvent(eEvents::EventEmberInTheWind, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventNextEmberInTheWind:
                    {
                        me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                        me->CastSpell(me, eSpells::EmberInTheWindNext, true);
                        m_Events.ScheduleEvent(eEvents::EventNextEmberInTheWind, 1 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_moldana_two_bladeAI(p_Creature);
        }
};

/// Iron Assembly Warden - 80400
class npc_foundry_iron_assembly_warden : public CreatureScript
{
    public:
        npc_foundry_iron_assembly_warden() : CreatureScript("npc_foundry_iron_assembly_warden") { }

        enum eSpells
        {
            WhirlingSteelAoE    = 162516,
            ShieldSlamDamage    = 162508,
            IronBulwark         = 162523
        };

        enum eEvents
        {
            EventWhirlingSteel = 1,
            EventShieldSlam,
            EventIronBulwark
        };

        enum eTalk
        {
            TalkAggro
        };

        struct npc_foundry_iron_assembly_wardenAI : public ScriptedAI
        {
            npc_foundry_iron_assembly_wardenAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                /// Holds a shield
                me->SetCanDualWield(false);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                Talk(eTalk::TalkAggro);

                m_Events.ScheduleEvent(eEvents::EventWhirlingSteel, urand(8 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventShieldSlam, urand(4 * TimeConstants::IN_MILLISECONDS, 6 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventIronBulwark, urand(15 * TimeConstants::IN_MILLISECONDS, 17 * TimeConstants::IN_MILLISECONDS));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventWhirlingSteel:
                    {
                        me->CastSpell(me, eSpells::WhirlingSteelAoE, true);
                        m_Events.ScheduleEvent(eEvents::EventWhirlingSteel, urand(18 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventShieldSlam:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::ShieldSlamDamage, true);

                        m_Events.ScheduleEvent(eEvents::EventShieldSlam, urand(18 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventIronBulwark:
                    {
                        me->CastSpell(me, eSpells::IronBulwark, true);
                        m_Events.ScheduleEvent(eEvents::EventIronBulwark, urand(30 * TimeConstants::IN_MILLISECONDS, 35 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_assembly_wardenAI(p_Creature);
        }
};

/// Thunderlord Beast-Tender - 80423
class npc_foundry_thunderlord_beast_tender : public CreatureScript
{
    public:
        npc_foundry_thunderlord_beast_tender() : CreatureScript("npc_foundry_thunderlord_beast_tender") { }

        enum eSpells
        {
            BestialWrath            = 162606,
            CallLightning           = 162633,
            ElectricalStormSearcher = 162653,
            ElectricalStormAT       = 162657
        };

        enum eEvents
        {
            EventBestialWrath = 1,
            EventCallLightning,
            EventElectricalStorm
        };

        enum eCreatures
        {
            StubbornIronhoof    = 80526,
            OrneryIronhoof      = 80534
        };

        struct npc_foundry_thunderlord_beast_tenderAI : public ScriptedAI
        {
            npc_foundry_thunderlord_beast_tenderAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            std::vector<uint64> m_IronhoofGuards;

            void Reset() override
            {
                m_Events.Reset();

                if (m_IronhoofGuards.empty())
                {
                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Creature* l_Stubborn = me->FindNearestCreature(eCreatures::StubbornIronhoof, 30.0f))
                            m_IronhoofGuards.push_back(l_Stubborn->GetGUID());

                        if (Creature* l_Ornery = me->FindNearestCreature(eCreatures::OrneryIronhoof, 30.0f))
                            m_IronhoofGuards.push_back(l_Ornery->GetGUID());
                    });
                }

                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventBestialWrath, urand(6 * TimeConstants::IN_MILLISECONDS, 8 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventCallLightning, urand(4 * TimeConstants::IN_MILLISECONDS, 6 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventElectricalStorm, urand(12 * TimeConstants::IN_MILLISECONDS, 14 * TimeConstants::IN_MILLISECONDS));
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::ElectricalStormSearcher)
                    me->CastSpell(p_Target, eSpells::ElectricalStormAT, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventBestialWrath:
                    {
                        if (!m_IronhoofGuards.empty())
                        {
                            if (Creature* l_Ironhoof = Creature::GetCreature(*me, m_IronhoofGuards[urand(0, m_IronhoofGuards.size() - 1)]))
                                me->CastSpell(l_Ironhoof, eSpells::BestialWrath, false);
                        }

                        m_Events.ScheduleEvent(eEvents::EventBestialWrath, urand(18 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventCallLightning:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::CallLightning, false);

                        m_Events.ScheduleEvent(eEvents::EventCallLightning, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventElectricalStorm:
                    {
                        me->CastSpell(me, eSpells::ElectricalStormSearcher, false);
                        m_Events.ScheduleEvent(eEvents::EventElectricalStorm, urand(14 * TimeConstants::IN_MILLISECONDS, 16 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_thunderlord_beast_tenderAI(p_Creature);
        }
};

/// Stubborn Ironhoof - 80526
class npc_foundry_stubborn_ironhoof : public CreatureScript
{
    public:
        npc_foundry_stubborn_ironhoof() : CreatureScript("npc_foundry_stubborn_ironhoof") { }

        enum eSpells
        {
            GoringSwipeAoE  = 162672,
            GroundSlamAoE   = 162675,
            SpiritBond      = 173789
        };

        enum eEvents
        {
            EventGoringSwipe = 1,
            EventGroundSlam
        };

        struct npc_foundry_stubborn_ironhoofAI : public ScriptedAI
        {
            npc_foundry_stubborn_ironhoofAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                me->CastSpell(me, eSpells::SpiritBond, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventGoringSwipe, urand(8 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventGroundSlam, urand(4 * TimeConstants::IN_MILLISECONDS, 6 * TimeConstants::IN_MILLISECONDS));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventGoringSwipe:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->SetFacingTo(me->GetAngle(l_Target));

                        AddTimedDelayedOperation(50, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::GoringSwipeAoE, false);
                        });

                        m_Events.ScheduleEvent(eEvents::EventGoringSwipe, urand(15 * TimeConstants::IN_MILLISECONDS, 17 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventGroundSlam:
                    {
                        me->CastSpell(me, eSpells::GroundSlamAoE, true);
                        m_Events.ScheduleEvent(eEvents::EventGroundSlam, urand(12 * TimeConstants::IN_MILLISECONDS, 14 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_stubborn_ironhoofAI(p_Creature);
        }
};

/// Ornery Ironhoof - 80534
class npc_foundry_ornery_ironhoof : public CreatureScript
{
    public:
        npc_foundry_ornery_ironhoof() : CreatureScript("npc_foundry_ornery_ironhoof") { }

        enum eSpells
        {
            GoringSwipeAoE  = 162672,
            GroundSlamAoE   = 162675,
            SpiritBond      = 173789
        };

        enum eEvents
        {
            EventGoringSwipe = 1,
            EventGroundSlam
        };

        struct npc_foundry_ornery_ironhoofAI : public ScriptedAI
        {
            npc_foundry_ornery_ironhoofAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                me->CastSpell(me, eSpells::SpiritBond, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventGoringSwipe, urand(8 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventGroundSlam, urand(4 * TimeConstants::IN_MILLISECONDS, 6 * TimeConstants::IN_MILLISECONDS));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventGoringSwipe:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->SetFacingTo(me->GetAngle(l_Target));

                        AddTimedDelayedOperation(50, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::GoringSwipeAoE, false);
                        });

                        m_Events.ScheduleEvent(eEvents::EventGoringSwipe, urand(15 * TimeConstants::IN_MILLISECONDS, 17 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventGroundSlam:
                    {
                        me->CastSpell(me, eSpells::GroundSlamAoE, true);
                        m_Events.ScheduleEvent(eEvents::EventGroundSlam, urand(12 * TimeConstants::IN_MILLISECONDS, 14 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_ornery_ironhoofAI(p_Creature);
        }
};

/// Iron Marksman - 80411
class npc_foundry_iron_marksman : public CreatureScript
{
    public:
        npc_foundry_iron_marksman() : CreatureScript("npc_foundry_iron_marksman") { }

        enum eSpells
        {
            Shoot               = 162777,
            IceTrap             = 162757,
            ScatterShotSearcher = 162749,
            ScatterShotTrigger  = 162748,
            BattleMorale        = 162902
        };

        enum eEvents
        {
            EventIceTrap = 1,
            EventScatterShot,
            EventBattleMorale
        };

        struct npc_foundry_iron_marksmanAI : public ScriptedAI
        {
            npc_foundry_iron_marksmanAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventIceTrap, urand(8 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventScatterShot, urand(10 * TimeConstants::IN_MILLISECONDS, 12 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventBattleMorale, urand(12 * TimeConstants::IN_MILLISECONDS, 14 * TimeConstants::IN_MILLISECONDS));
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::ScatterShotSearcher)
                    me->CastSpell(p_Target, eSpells::ScatterShotTrigger, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventIceTrap:
                    {
                        if (Player* l_Target = SelectRangedTarget())
                            me->CastSpell(*l_Target, eSpells::IceTrap, true);

                        m_Events.ScheduleEvent(eEvents::EventIceTrap, urand(14 * TimeConstants::IN_MILLISECONDS, 16 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventScatterShot:
                    {
                        me->CastSpell(me, eSpells::ScatterShotSearcher, true);
                        m_Events.ScheduleEvent(eEvents::EventScatterShot, urand(10 * TimeConstants::IN_MILLISECONDS, 12 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eEvents::EventBattleMorale:
                    {
                        me->CastSpell(me, eSpells::BattleMorale, true);
                        m_Events.ScheduleEvent(eEvents::EventBattleMorale, urand(18 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoSpellAttackIfReady(eSpells::Shoot);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_marksmanAI(p_Creature);
        }
};

/// Markog Aba'dir <The Iron Enforcer> - 80409
class npc_foundry_markog_abadir : public CreatureScript
{
    public:
        npc_foundry_markog_abadir() : CreatureScript("npc_foundry_markog_abadir") { }

        enum eSpells
        {
            ColossalRoar    = 162772,
            Slam            = 175291,
            Stun            = 9179
        };

        enum eEvents
        {
            EventColossalRoar = 1,
            EventSlam
        };

        enum eTalk
        {
            TalkStun
        };

        struct npc_foundry_markog_abadirAI : public ScriptedAI
        {
            npc_foundry_markog_abadirAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            int32 m_StacksResetPct;

            void Reset() override
            {
                m_Events.Reset();

                m_StacksResetPct = 70;
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventColossalRoar, urand(6 * TimeConstants::IN_MILLISECONDS, 8 * TimeConstants::IN_MILLISECONDS));
                m_Events.ScheduleEvent(eEvents::EventSlam, urand(8 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS));
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_StacksResetPct <= 0)
                    return;

                if (me->HealthBelowPctDamaged(m_StacksResetPct, p_Damage))
                {
                    if (m_StacksResetPct >= 70)
                        m_StacksResetPct = 40;
                    else
                        m_StacksResetPct = 0;

                    me->RemoveAura(eSpells::ColossalRoar);

                    me->CastSpell(me, eSpells::Stun, true);

                    Talk(eTalk::TalkStun);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventColossalRoar:
                    {
                        me->CastSpell(me, eSpells::ColossalRoar, true);
                        m_Events.ScheduleEvent(eEvents::EventColossalRoar, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSlam:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, eSpells::Slam, false);

                        m_Events.ScheduleEvent(eEvents::EventSlam, urand(14 * TimeConstants::IN_MILLISECONDS, 16 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_markog_abadirAI(p_Creature);
        }
};

/// Grom'kar Man-at-Arms - 78832
/// Grom'kar Man-at-Arms - 77687
class npc_foundry_gromkar_man_at_arms : public CreatureScript
{
    public:
        npc_foundry_gromkar_man_at_arms() : CreatureScript("npc_foundry_gromkar_man_at_arms") { }

        enum eSpells
        {
            SpellIronBellow             = 163753,
            SpellIronBellowDmg          = 163754,

            SpellRecklessSlashCharge    = 163750,
            SpellRecklessSlashDmg       = 163751
        };

        enum eEvents
        {
            EventIronBellow = 1,
            EventRecklessSlash
        };

        struct npc_foundry_gromkar_man_at_armsAI : public ScriptedAI
        {
            npc_foundry_gromkar_man_at_armsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();

                if (p_Creature->IsNearPosition(&g_GromkarManAtArmsIntroLeftPos, 5.0f))
                    m_IsThogarIntro = true;
                else if (p_Creature->IsNearPosition(&g_GromkarManAtArmsIntroRightPos, 5.0f))
                    m_IsThogarIntro = true;
                else
                    m_IsThogarIntro = false;
            }

            EventMap m_Events;

            InstanceScript* m_Instance;

            bool m_IsThogarIntro;

            void Reset() override
            {
                m_Events.Reset();

                /// Wears a shield
                me->SetCanDualWield(false);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventIronBellow, 1);
                m_Events.ScheduleEvent(eEvents::EventRecklessSlash, 3 * TimeConstants::IN_MILLISECONDS);

                if (m_IsThogarIntro && m_Instance != nullptr)
                {
                    if (!m_Instance->GetData(eFoundryDatas::DataThogarIntroStarted))
                    {
                        m_Instance->SetData(eFoundryDatas::DataThogarIntroStarted, 0);

                        /// Must be done for train spawning
                        me->GetMap()->SetObjectVisibility(500.0f);

                        SummonTrain(me, eThogarTrains::IntroTroopsTrain, eThogarActions::IntroBegin, false);
                    }
                    else if (Creature* l_Wheels = me->FindNearestCreature(eThogarCreatures::TrainWheels, 100.0f))
                    {
                        if (l_Wheels->IsAIEnabled)
                        {
                            l_Wheels->AI()->SetGUID(me->GetGUID());
                            l_Wheels->AI()->DoAction(eThogarActions::IntroEnd);
                        }
                    }
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                CreatureAI::JustSummoned(p_Summon);

                if (m_Instance == nullptr)
                    return;

                switch (p_Summon->GetEntry())
                {
                    case eFoundryCreatures::IronRaider:
                    {
                        if (Creature* l_Thogar = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BossOperatorThogar)))
                        {
                            if (l_Thogar->IsAIEnabled)
                                l_Thogar->AI()->SetGUID(p_Summon->GetGUID(), 0);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance != nullptr && me->GetEntry() == eThogarCreatures::ManAtArmsIntro)
                {
                    if (Creature* l_Thogar = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BossOperatorThogar)))
                    {
                        if (l_Thogar->IsAIEnabled)
                            l_Thogar->AI()->SetGUID(me->GetGUID(), 1);
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellIronBellow:
                    {
                        me->CastSpell(p_Target, eSpells::SpellIronBellowDmg, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == eSpells::SpellRecklessSlashCharge)
                    me->CastSpell(me, eSpells::SpellRecklessSlashDmg, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventIronBellow:
                    {
                        me->CastSpell(me, eSpells::SpellIronBellow, false);
                        m_Events.ScheduleEvent(eEvents::EventIronBellow, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRecklessSlash:
                    {
                        if (Player* l_Target = SelectMeleeTarget(true))
                            me->CastSpell(*l_Target, eSpells::SpellRecklessSlashCharge, true);
                        m_Events.ScheduleEvent(eEvents::EventRecklessSlash, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_gromkar_man_at_armsAI(p_Creature);
        }
};

/// Iron Raider - 81197
class npc_foundry_iron_raider : public CreatureScript
{
    public:
        npc_foundry_iron_raider() : CreatureScript("npc_foundry_iron_raider") { }

        enum eSpell
        {
            SpellThrowGrenade = 156294
        };

        enum eEvent
        {
            EventThrowGrenade = 1
        };

        struct npc_foundry_iron_raiderAI : public ScriptedAI
        {
            npc_foundry_iron_raiderAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvent::EventThrowGrenade, urand(2 * TimeConstants::IN_MILLISECONDS, 5 * TimeConstants::IN_MILLISECONDS));
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* l_InstanceScript = me->GetInstanceScript())
                {
                    if (Creature* l_Thogar = Creature::GetCreature(*me, l_InstanceScript->GetData64(eFoundryCreatures::BossOperatorThogar)))
                    {
                        if (l_Thogar->IsAIEnabled)
                            l_Thogar->AI()->SetGUID(me->GetGUID(), 1);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventThrowGrenade:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(*l_Target, eSpell::SpellThrowGrenade, false);
                        m_Events.ScheduleEvent(eEvent::EventThrowGrenade, urand(5 * TimeConstants::IN_MILLISECONDS, 7 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_raiderAI(p_Creature);
        }
};

/// Iron Crack-Shot - 81315
class npc_foundry_iron_crack_shot : public CreatureScript
{
    public:
        npc_foundry_iron_crack_shot() : CreatureScript("npc_foundry_iron_crack_shot") { }

        enum eSpells
        {
            SpellShootRifle     = 156270,

            SpellThrowGrenade   = 156294
        };

        enum eEvent
        {
            EventThrowGrenade = 1
        };

        struct npc_foundry_iron_crack_shotAI : public ScriptedAI
        {
            npc_foundry_iron_crack_shotAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvent::EventThrowGrenade, urand(2 * TimeConstants::IN_MILLISECONDS, 5 * TimeConstants::IN_MILLISECONDS));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventThrowGrenade:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(*l_Target, eSpells::SpellThrowGrenade, false);
                        m_Events.ScheduleEvent(eEvent::EventThrowGrenade, urand(5 * TimeConstants::IN_MILLISECONDS, 7 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    default:
                        break;
                }

                DoSpellAttackIfReady(eSpells::SpellShootRifle);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_crack_shotAI(p_Creature);
        }
};

/// Grom'kar Firemender - 87841
class npc_foundry_gromkar_firemender : public CreatureScript
{
    public:
        npc_foundry_gromkar_firemender() : CreatureScript("npc_foundry_gromkar_firemender") { }

        enum eSpells
        {
            SpellCauterizingBoltSearcher    = 160177,
            SpellCauterizingBoltAoE         = 160140
        };

        enum eEvent
        {
            EventCauterizingBolt = 1
        };

        struct npc_foundry_gromkar_firemenderAI : public ScriptedAI
        {
            npc_foundry_gromkar_firemenderAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                /// Wears a shield
                me->SetCanDualWield(false);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvent::EventCauterizingBolt, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::SpellCauterizingBoltSearcher)
                    me->CastSpell(*p_Target, eSpells::SpellCauterizingBoltAoE, false);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventCauterizingBolt:
                    {
                        me->CastSpell(me, eSpells::SpellCauterizingBoltSearcher, true);
                        m_Events.ScheduleEvent(eEvent::EventCauterizingBolt, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_gromkar_firemenderAI(p_Creature);
        }
};

/// Grievous Mortal Wounds - 175624
class spell_foundry_grievous_mortal_wounds : public SpellScriptLoader
{
    public:
        spell_foundry_grievous_mortal_wounds() : SpellScriptLoader("spell_foundry_grievous_mortal_wounds") { }

        class spell_foundry_grievous_mortal_wounds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_grievous_mortal_wounds_AuraScript)

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (Unit* l_Target = GetTarget())
                {
                    /// Inflicts Physical damage and reduces healing received by -5% until the target is healed to full health.
                    if (l_Target->IsFullHealth())
                        p_AurEff->GetBase()->SetDuration(1);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_foundry_grievous_mortal_wounds_AuraScript::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_grievous_mortal_wounds_AuraScript();
        }
};

/// Spinning Blade - 175644
class spell_foundry_spinning_blade : public SpellScriptLoader
{
    public:
        spell_foundry_spinning_blade() : SpellScriptLoader("spell_foundry_spinning_blade") { }

        class spell_foundry_spinning_blade_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_spinning_blade_AuraScript)

            enum eSpell
            {
                SpinningBladeDoT = 175643
            };

            uint32 m_CheckTimer;

            bool Load() override
            {
                m_CheckTimer = 200;
                return true;
            }

            void OnUpdate(uint32 p_Diff)
            {
                if (m_CheckTimer)
                {
                    if (m_CheckTimer <= p_Diff)
                    {
                        if (Unit* l_Caster = GetCaster())
                        {
                            std::list<Unit*> l_TargetList;
                            float l_Radius = 8.0f;

                            JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                            l_TargetList.remove(l_Caster);

                            for (Unit* l_Iter : l_TargetList)
                            {
                                if (l_Iter->GetDistance(l_Caster) <= 1.0f)
                                {
                                    if (!l_Iter->HasAura(eSpell::SpinningBladeDoT))
                                        l_Iter->CastSpell(l_Iter, eSpell::SpinningBladeDoT, true, nullptr, nullptr, l_Caster->GetGUID());
                                }
                                else
                                {
                                    if (l_Iter->HasAura(eSpell::SpinningBladeDoT))
                                        l_Iter->RemoveAura(eSpell::SpinningBladeDoT);
                                }
                            }
                        }

                        m_CheckTimer = 200;
                    }
                    else
                        m_CheckTimer -= p_Diff;
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_foundry_spinning_blade_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_spinning_blade_AuraScript();
        }
};

/// Slag Breath - 175752
class spell_foundry_slag_breath : public SpellScriptLoader
{
    public:
        spell_foundry_slag_breath() : SpellScriptLoader("spell_foundry_slag_breath") { }

        class spell_foundry_slag_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_foundry_slag_breath_SpellScript)

            enum eSpell
            {
                TargetRestrict = 24223
            };

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                if (l_Restriction == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                float l_Angle = 2 * M_PI / 360 * l_Restriction->ConeAngle;
                p_Targets.remove_if([l_Caster, l_Angle](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (!p_Object->isInFront(l_Caster, l_Angle))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_foundry_slag_breath_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_foundry_slag_breath_SpellScript();
        }
};

/// Animate Slag - 175088
class spell_foundry_animate_slag : public SpellScriptLoader
{
    public:
        spell_foundry_animate_slag() : SpellScriptLoader("spell_foundry_animate_slag") { }

        class spell_foundry_animate_slag_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_animate_slag_AuraScript)

            enum eSpell
            {
                AnimateSlagMissile = 175089
            };

            enum eAction
            {
                ActionRescheduleSlag
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (Unit* l_Caster = GetCaster())
                        l_Target->CastSpell(l_Caster, eSpell::AnimateSlagMissile, true);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE || GetCaster() == nullptr)
                    return;

                if (Creature* l_Creature = GetCaster()->ToCreature())
                {
                    if (!l_Creature->IsAIEnabled)
                        return;

                    l_Creature->AI()->DoAction(eAction::ActionRescheduleSlag);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_foundry_animate_slag_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_foundry_animate_slag_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_animate_slag_AuraScript();
        }
};

/// Gronnling Smash - 169402
class spell_foundry_gronnling_smash : public SpellScriptLoader
{
    public:
        spell_foundry_gronnling_smash() : SpellScriptLoader("spell_foundry_gronnling_smash") { }

        class spell_foundry_gronnling_smash_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_gronnling_smash_AuraScript)

            enum eSpell
            {
                GronnlingSmashAoE = 169401
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(l_Target, eSpell::GronnlingSmashAoE, TriggerCastFlags::TRIGGERED_IGNORE_CAST_IN_PROGRESS);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_foundry_gronnling_smash_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_gronnling_smash_AuraScript();
        }
};

/// Shattering Charge - 159518
class spell_foundry_shattering_charge : public SpellScriptLoader
{
    public:
        spell_foundry_shattering_charge() : SpellScriptLoader("spell_foundry_shattering_charge") { }

        class spell_foundry_shattering_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_shattering_charge_AuraScript)

            enum eSpell
            {
                ShatteringCharge = 159520
            };

            uint32 m_CheckTimer;

            bool Load() override
            {
                m_CheckTimer = 200;
                return true;
            }

            void OnUpdate(uint32 p_Diff)
            {
                if (m_CheckTimer)
                {
                    if (m_CheckTimer <= p_Diff)
                    {
                        if (Unit* l_Caster = GetCaster())
                        {
                            std::list<Unit*> l_TargetList;
                            float l_Radius = 8.0f;

                            JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                            l_TargetList.remove(l_Caster);

                            for (Unit* l_Iter : l_TargetList)
                            {
                                if (l_Iter->GetDistance(l_Caster) <= 2.5f)
                                    l_Caster->CastSpell(l_Iter, eSpell::ShatteringCharge, true);
                            }
                        }

                        m_CheckTimer = 200;
                    }
                    else
                        m_CheckTimer -= p_Diff;
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_foundry_shattering_charge_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_shattering_charge_AuraScript();
        }
};

/// Ignite - 156345
class spell_foundry_ignite_aura : public SpellScriptLoader
{
    public:
        spell_foundry_ignite_aura() : SpellScriptLoader("spell_foundry_ignite_aura") { }

        class spell_foundry_ignite_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_ignite_aura_AuraScript)

            enum eSpell
            {
                IgniteAoE = 156346
            };

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(*l_Target, eSpell::IgniteAoE, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_foundry_ignite_aura_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_ignite_aura_AuraScript();
        }
};

/// Blast Wave - 158424
class spell_foundry_blast_wave : public SpellScriptLoader
{
    public:
        spell_foundry_blast_wave() : SpellScriptLoader("spell_foundry_blast_wave") { }

        class spell_foundry_blast_wave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_foundry_blast_wave_SpellScript)

            enum eGameObjects
            {
                VolcanicBomb    = 227616,
                IceBlock        = 201722
            };

            void SelectIceBlocks(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.clear();

                if (Unit* l_Caster = GetCaster())
                {
                    for (uint8 l_I = 0; l_I < (uint8)EFFECT_2; ++l_I)
                    {
                        float l_Radius = GetSpellInfo()->Effects[l_I].CalcRadius(l_Caster);

                        std::list<GameObject*> l_GoBList;
                        l_Caster->GetGameObjectListWithEntryInGrid(l_GoBList, l_I == 0 ? eGameObjects::IceBlock : eGameObjects::VolcanicBomb, l_Radius);

                        for (GameObject* l_Gob : l_GoBList)
                            p_Targets.push_back(l_Gob);
                    }
                }
            }

            void HandleActivateObject(SpellEffIndex /*p_EffIndex*/)
            {
                if (GameObject* l_GameObject = GetHitGObj())
                    l_GameObject->Delete();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_foundry_blast_wave_SpellScript::SelectIceBlocks, EFFECT_0, TARGET_GAMEOBJECT_SRC_AREA);
                OnEffectHitTarget += SpellEffectFn(spell_foundry_blast_wave_SpellScript::HandleActivateObject, EFFECT_0, SPELL_EFFECT_ACTIVATE_OBJECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_foundry_blast_wave_SpellScript();
        }
};

/// Rending Slash - 160092
class spell_foundry_rending_slash : public SpellScriptLoader
{
    public:
        spell_foundry_rending_slash() : SpellScriptLoader("spell_foundry_rending_slash") { }

        class spell_foundry_rending_slash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_foundry_rending_slash_SpellScript)

            enum eSpell
            {
                TargetRestrict = 22509
            };

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                if (l_Restriction == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                float l_Angle = 2 * M_PI / 360 * l_Restriction->ConeAngle;
                p_Targets.remove_if([l_Caster, l_Angle](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (!p_Object->isInFront(l_Caster, l_Angle))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_foundry_rending_slash_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_foundry_rending_slash_SpellScript();
        }
};

/// Crushing Slam - 163182
class spell_foundry_crushing_slam : public SpellScriptLoader
{
    public:
        spell_foundry_crushing_slam() : SpellScriptLoader("spell_foundry_crushing_slam") { }

        class spell_foundry_crushing_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_foundry_crushing_slam_SpellScript)

            enum eSpell
            {
                TargetRestrict = 20831
            };

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                if (l_Restriction == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                float l_Angle = 2 * M_PI / 360 * l_Restriction->ConeAngle;
                p_Targets.remove_if([l_Caster, l_Angle](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (!p_Object->isInFront(l_Caster, l_Angle))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_foundry_crushing_slam_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_foundry_crushing_slam_SpellScript();
        }
};

/// Ember in the Wind - 177860
class spell_foundry_ember_in_the_wind_damage : public SpellScriptLoader
{
    public:
        spell_foundry_ember_in_the_wind_damage() : SpellScriptLoader("spell_foundry_ember_in_the_wind_damage") { }

        class spell_foundry_ember_in_the_wind_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_foundry_ember_in_the_wind_damage_SpellScript)

            void HandleDamage()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMap()->GetDifficultyID() != Difficulty::DifficultyRaidHeroic)
                        return;

                    /// No hotfix on retail, even on newest DBCs
                    SetHitDamage(GetHitDamage() / 10);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_foundry_ember_in_the_wind_damage_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_foundry_ember_in_the_wind_damage_SpellScript();
        }
};

/// Electrical Storm (Searcher) - 162653
class spell_foundry_electrical_storm : public SpellScriptLoader
{
    public:
        spell_foundry_electrical_storm() : SpellScriptLoader("spell_foundry_electrical_storm") { }

        class spell_foundry_electrical_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_foundry_electrical_storm_SpellScript)

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    if (!p_Object->ToPlayer()->IsRangedDamageDealer(l_Caster->ToCreature()))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_foundry_electrical_storm_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_foundry_electrical_storm_SpellScript();
        }
};

/// Cauterizing Bolt - 160140
class spell_foundry_cauterizing_bolt : public SpellScriptLoader
{
    public:
        spell_foundry_cauterizing_bolt() : SpellScriptLoader("spell_foundry_cauterizing_bolt") { }

        class spell_foundry_cauterizing_bolt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_cauterizing_bolt_AuraScript)

            enum eSpell
            {
                CauterizingBoltHeal = 156323
            };

            enum eVisual
            {
                VisualKitID = 42432
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    int32 l_Damage = l_Target->GetMap()->IsLFR() ? 5 : 20;

                    /// Heal is 50% more powerful than damage
                    AddPct(l_Damage, 50);

                    /// 10 Tics, 10% per tic
                    l_Damage /= 10;

                    l_Target->SendPlaySpellVisualKit(eVisual::VisualKitID, 0);
                    l_Target->CastCustomSpell(l_Target, eSpell::CauterizingBoltHeal, &l_Damage, nullptr, nullptr, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_foundry_cauterizing_bolt_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_cauterizing_bolt_AuraScript();
        }
};

/// Acidback Puddle - 159121
class areatrigger_foundry_acidback_puddle : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_acidback_puddle() : AreaTriggerEntityScript("areatrigger_foundry_acidback_puddle") { }

        enum eSpell
        {
            AcidbackPuddleDoT = 159686
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 15.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Unit->GetDistance(p_AreaTrigger) <= 2.8f)
                    {
                        if (!l_Unit->HasAura(eSpell::AcidbackPuddleDoT))
                            l_Unit->CastSpell(l_Unit, eSpell::AcidbackPuddleDoT, true);
                    }
                    else if (!l_Unit->FindNearestAreaTrigger(p_AreaTrigger->GetSpellId(), 2.8f))
                    {
                        if (l_Unit->HasAura(eSpell::AcidbackPuddleDoT))
                            l_Unit->RemoveAura(eSpell::AcidbackPuddleDoT);
                    }
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 6.5f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    std::list<AreaTrigger*> l_ATList;
                    l_Unit->GetAreatriggerListInRange(l_ATList, 2.8f);

                    if (l_ATList.empty())
                    {
                        if (l_Unit->HasAura(eSpell::AcidbackPuddleDoT))
                            l_Unit->RemoveAura(eSpell::AcidbackPuddleDoT);
                    }

                    uint32 l_SpellID = p_AreaTrigger->GetSpellId();
                    l_ATList.remove_if([this, l_SpellID](AreaTrigger* p_AreaTrigger) -> bool
                    {
                        if (p_AreaTrigger == nullptr || p_AreaTrigger->GetSpellId() != l_SpellID)
                            return true;

                        return false;
                    });

                    if (l_ATList.empty())
                    {
                        if (l_Unit->HasAura(eSpell::AcidbackPuddleDoT))
                            l_Unit->RemoveAura(eSpell::AcidbackPuddleDoT);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_acidback_puddle();
        }
};

/// Fire Bomb - 160259
class areatrigger_foundry_fire_bomb : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_fire_bomb() : AreaTriggerEntityScript("areatrigger_foundry_fire_bomb") { }

        enum eSpell
        {
            FireBombDoT = 160260
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 30.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Unit->GetDistance(p_AreaTrigger) <= 12.0f)
                    {
                        if (!l_Unit->HasAura(eSpell::FireBombDoT, l_Caster->GetGUID()))
                            l_Caster->CastSpell(l_Unit, eSpell::FireBombDoT, true);
                    }
                    else
                    {
                        if (l_Unit->HasAura(eSpell::FireBombDoT, l_Caster->GetGUID()))
                            l_Unit->RemoveAura(eSpell::FireBombDoT, l_Caster->GetGUID());
                    }
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 30.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Unit->HasAura(eSpell::FireBombDoT, l_Caster->GetGUID()))
                        l_Unit->RemoveAura(eSpell::FireBombDoT, l_Caster->GetGUID());
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_fire_bomb();
        }
};

/// Electrical Storm - 162657
class areatrigger_foundry_electrical_storm : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_electrical_storm() : AreaTriggerEntityScript("areatrigger_foundry_electrical_storm") { }

        enum eSpell
        {
            ElectricalStormDoT = 162663
        };

        std::set<uint64> m_AffectedPlayers;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 3.75f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                std::set<uint64> l_Targets;

                for (Unit* l_Iter : l_TargetList)
                {
                    l_Targets.insert(l_Iter->GetGUID());

                    if (!l_Iter->HasAura(eSpell::ElectricalStormDoT, l_Caster->GetGUID()))
                    {
                        m_AffectedPlayers.insert(l_Iter->GetGUID());
                        l_Caster->CastSpell(l_Iter, eSpell::ElectricalStormDoT, true);
                    }
                }

                for (std::set<uint64>::iterator l_Iter = m_AffectedPlayers.begin(); l_Iter != m_AffectedPlayers.end();)
                {
                    if (l_Targets.find((*l_Iter)) != l_Targets.end())
                    {
                        ++l_Iter;
                        continue;
                    }

                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, (*l_Iter)))
                    {
                        l_Iter = m_AffectedPlayers.erase(l_Iter);
                        l_Unit->RemoveAura(eSpell::ElectricalStormDoT, l_Caster->GetGUID());

                        continue;
                    }

                    ++l_Iter;
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                for (uint64 l_Guid : m_AffectedPlayers)
                {
                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, l_Guid))
                        l_Unit->RemoveAura(eSpell::ElectricalStormDoT, l_Caster->GetGUID());
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_electrical_storm();
        }
};

/// Ice Trap - 162757
class areatrigger_foundry_ice_trap : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_ice_trap() : AreaTriggerEntityScript("areatrigger_foundry_ice_trap") { }

        enum eSpell
        {
            IceTrapEffect = 162761
        };

        std::set<uint64> m_AffectedPlayers;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 3.75f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                std::set<uint64> l_Targets;

                for (Unit* l_Iter : l_TargetList)
                {
                    l_Targets.insert(l_Iter->GetGUID());

                    if (!l_Iter->HasAura(eSpell::IceTrapEffect, l_Caster->GetGUID()))
                    {
                        m_AffectedPlayers.insert(l_Iter->GetGUID());
                        l_Caster->CastSpell(l_Iter, eSpell::IceTrapEffect, true);
                    }
                }

                for (std::set<uint64>::iterator l_Iter = m_AffectedPlayers.begin(); l_Iter != m_AffectedPlayers.end();)
                {
                    if (l_Targets.find((*l_Iter)) != l_Targets.end())
                    {
                        ++l_Iter;
                        continue;
                    }

                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, (*l_Iter)))
                    {
                        l_Iter = m_AffectedPlayers.erase(l_Iter);
                        l_Unit->RemoveAura(eSpell::IceTrapEffect, l_Caster->GetGUID());

                        continue;
                    }

                    ++l_Iter;
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                for (uint64 l_Guid : m_AffectedPlayers)
                {
                    if (Unit* l_Unit = Unit::GetUnit(*l_Caster, l_Guid))
                        l_Unit->RemoveAura(eSpell::IceTrapEffect, l_Caster->GetGUID());
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_ice_trap();
        }
};

/// First Floor Trap - 10276
class areatrigger_at_foundry_first_floor_trap : public AreaTriggerScript
{
    public:
        areatrigger_at_foundry_first_floor_trap() : AreaTriggerScript("areatrigger_at_foundry_first_floor_trap") { }

        enum eSpell
        {
            KromogsFury = 175331
        };

        enum eCreature
        {
            KromogsWrath = 87727
        };

        void OnEnter(Player* p_Player, AreaTriggerEntry const* /*p_AreaTrigger*/) override
        {
            float l_MaxZ = 308.45f;
            float l_MinZ = 274.45f;

            std::list<Creature*> l_WrathList;
            p_Player->GetCreatureListWithEntryInGrid(l_WrathList, eCreature::KromogsWrath, 150.0f);

            if (l_WrathList.empty())
                return;

            l_WrathList.remove_if([this, l_MaxZ, l_MinZ](Creature* p_Creature) -> bool
            {
                if (p_Creature == nullptr || p_Creature->GetPositionZ() >= l_MaxZ || p_Creature->GetPositionZ() <= l_MinZ)
                    return true;

                return false;
            });

            for (Creature* l_Wrath : l_WrathList)
            {
                if (l_Wrath->HasAura(eSpell::KromogsFury))
                    continue;

                l_Wrath->CastSpell(l_Wrath, eSpell::KromogsFury, true);
            }
        }
};

/// Second Floor Trap - 10277
class areatrigger_at_foundry_second_floor_trap : public AreaTriggerScript
{
    public:
        areatrigger_at_foundry_second_floor_trap() : AreaTriggerScript("areatrigger_at_foundry_second_floor_trap") { }

        enum eSpell
        {
            KromogsFury = 175331
        };

        enum eCreature
        {
            KromogsWrath = 87727
        };

        void OnEnter(Player* p_Player, AreaTriggerEntry const* /*p_AreaTrigger*/) override
        {
            float l_MaxZ = 266.06f;
            float l_MinZ = 227.40f;

            std::list<Creature*> l_WrathList;
            p_Player->GetCreatureListWithEntryInGrid(l_WrathList, eCreature::KromogsWrath, 150.0f);

            if (l_WrathList.empty())
                return;

            l_WrathList.remove_if([this, l_MaxZ, l_MinZ](Creature* p_Creature) -> bool
            {
                if (p_Creature == nullptr || p_Creature->GetPositionZ() >= l_MaxZ || p_Creature->GetPositionZ() <= l_MinZ)
                    return true;

                return false;
            });

            for (Creature* l_Wrath : l_WrathList)
            {
                if (l_Wrath->HasAura(eSpell::KromogsFury))
                    continue;

                l_Wrath->CastSpell(l_Wrath, eSpell::KromogsFury, true);
            }
        }
};

/// Hans'gar & Franzok Entrance - 9998
class areatrigger_at_foundry_hansgar_and_franzok_entrance : public AreaTriggerScript
{
    public:
        areatrigger_at_foundry_hansgar_and_franzok_entrance() : AreaTriggerScript("areatrigger_at_foundry_hansgar_and_franzok_entrance") { }

        enum eAction
        {
            ActionIntro
        };

        void OnEnter(Player* p_Player, AreaTriggerEntry const* /*p_AreaTrigger*/) override
        {
            if (InstanceScript* l_Instance = p_Player->GetInstanceScript())
            {
                if (Creature* l_Hansgar = Creature::GetCreature(*p_Player, l_Instance->GetData64(eFoundryCreatures::BossHansgar)))
                {
                    if (l_Hansgar->IsAIEnabled)
                        l_Hansgar->AI()->DoAction(eAction::ActionIntro);
                }

                if (Creature* l_Franzok = Creature::GetCreature(*p_Player, l_Instance->GetData64(eFoundryCreatures::BossFranzok)))
                {
                    if (l_Franzok->IsAIEnabled)
                        l_Franzok->AI()->DoAction(eAction::ActionIntro);
                }
            }
        }
};

/// Hans'gar & Franzok Exit - 9999
class areatrigger_at_foundry_hansgar_and_franzok_exit : public AreaTriggerScript
{
    public:
        areatrigger_at_foundry_hansgar_and_franzok_exit() : AreaTriggerScript("areatrigger_at_foundry_hansgar_and_franzok_exit")
        {
            m_Activated = false;
        }

        enum eAction
        {
            ActionIntro
        };

        enum eCreature
        {
            IronTaskmaster = 80708
        };

        bool m_Activated;

        void OnEnter(Player* p_Player, AreaTriggerEntry const* /*p_AreaTrigger*/) override
        {
            if (m_Activated)
                return;

            if (Creature* l_Taskmaster = p_Player->FindNearestCreature(eCreature::IronTaskmaster, 80.0f))
            {
                if (l_Taskmaster->IsAIEnabled)
                    l_Taskmaster->AI()->DoAction(eAction::ActionIntro);
            }

            m_Activated = true;
        }
};

#ifndef __clang_analyzer__
void AddSC_blackrock_foundry()
{
    /// NPCs
    new npc_foundry_iron_flame_binder();
    new npc_foundry_living_flames();
    new npc_foundry_workshop_guardian();
    new npc_foundry_spinning_blade();
    new npc_foundry_ogron_hauler();
    new npc_foundry_ironworker();
    new npc_foundry_flame_jets();
    new npc_foundry_iron_slag_shaper();
    new npc_foundry_slagshop_worker();
    new npc_foundry_slagshop_brute();
    new npc_foundry_iron_journeyman();
    new npc_foundry_gronnling_laborer();
    new npc_foundry_karnor_the_cruel();
    new npc_foundry_iron_laborer_cosmetic();
    new npc_foundry_iron_laborer();
    new npc_foundry_darkshard_acidback();
    new npc_foundry_darkshard_gnasher();
    new npc_foundry_darkshard_crystalback();
    new npc_foundry_slag_behemoth();
    new npc_foundry_blackrock_enforcer();
    new npc_foundry_blackrock_forge_specialist();
    new npc_foundry_iron_taskmaster();
    new npc_foundry_iron_flametwister();
    new npc_foundry_iron_smith();
    new npc_foundry_flame_vents();
    new npc_foundry_enchanted_armament();
    new npc_foundry_moldana_two_blade();
    new npc_foundry_iron_assembly_warden();
    new npc_foundry_thunderlord_beast_tender();
    new npc_foundry_stubborn_ironhoof();
    new npc_foundry_ornery_ironhoof();
    new npc_foundry_iron_marksman();
    new npc_foundry_markog_abadir();
    new npc_foundry_gromkar_man_at_arms();
    new npc_foundry_iron_raider();
    new npc_foundry_iron_crack_shot();
    new npc_foundry_gromkar_firemender();

    /// Spells
    new spell_foundry_grievous_mortal_wounds();
    new spell_foundry_spinning_blade();
    new spell_foundry_slag_breath();
    new spell_foundry_animate_slag();
    new spell_foundry_gronnling_smash();
    new spell_foundry_shattering_charge();
    new spell_foundry_ignite_aura();
    new spell_foundry_blast_wave();
    new spell_foundry_rending_slash();
    new spell_foundry_crushing_slam();
    new spell_foundry_ember_in_the_wind_damage();
    new spell_foundry_electrical_storm();
    new spell_foundry_cauterizing_bolt();

    /// GameObjects

    /// AreaTriggers (spell)
    new areatrigger_foundry_acidback_puddle();
    new areatrigger_foundry_fire_bomb();
    new areatrigger_foundry_electrical_storm();
    new areatrigger_foundry_ice_trap();

    /// AreaTriggers (world)
    new areatrigger_at_foundry_first_floor_trap();
    new areatrigger_at_foundry_second_floor_trap();
    new areatrigger_at_foundry_hansgar_and_franzok_entrance();
    new areatrigger_at_foundry_hansgar_and_franzok_exit();
}
#endif
