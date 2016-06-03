////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

# include "boss_operator_thogar.hpp"

/// Operator Thogar <Grimrail Overseer> - 76906
class boss_operator_thogar : public CreatureScript
{
    public:
        boss_operator_thogar() : CreatureScript("boss_operator_thogar") { }

        enum eSpells
        {
            /// Enkindle
            SpellEnkindle                       = 155921,
            /// Prototype Pulse Grenade
            SpellPrototypePulseGrenade          = 155864,
            SpellPrototypePulseGrenadeMissile   = 165190,
            SpellPrototypePulseGrenadeAT        = 165194,
            /// Misc
            SpellThogarBonus                    = 177537
        };

        enum eEvents
        {
            EventBerserker = 1,
            EventEnkindle,
            EventPrototypePulseGrenade,
            EventSummonTrain
        };

        enum eTimers
        {
            TimerBerserker      = 8 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS,
            TimerEnkindle       = 16 * TimeConstants::IN_MILLISECONDS,
            TimerPulseGrenade   = 7 * TimeConstants::IN_MILLISECONDS,
            TimerPulseGrenadeCD = 12 * TimeConstants::IN_MILLISECONDS
        };

        enum eGuidTypes
        {
            IntroAdd,
            IntroRemove
        };

        enum eIntroStates : uint8
        {
            IronRaiders,
            GunnerySergeants,
            IntroEnd
        };

        enum eMove
        {
            MoveJump = 5
        };

        struct boss_operator_thogarAI : public BossAI
        {
            boss_operator_thogarAI(Creature* p_Creature) : BossAI(p_Creature, eFoundryDatas::DataOperatorThogar)
            {
                m_Instance      = p_Creature->GetInstanceScript();
                m_IntroDone     = false;
                m_IntroState    = eIntroStates::IronRaiders;
            }

            InstanceScript* m_Instance;

            EventMap m_Events;

            uint8 m_IntroState;
            bool m_IntroDone;
            std::set<uint64> m_IntroTrashes;

            uint8 m_TrainID;

            void Reset() override
            {
                m_Events.Reset();

                _Reset();

                me->RemoveAura(eFoundrySpells::Berserker);

                if (!m_IntroDone)
                {
                    AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        m_IntroTrashes.clear();

                        std::list<Creature*> l_AddsList;
                        me->GetCreatureListWithEntryInGrid(l_AddsList, eFoundryCreatures::GromkarManAtArms, 110.0f);

                        uint8 l_Count = 0;
                        for (Creature* l_Add : l_AddsList)
                        {
                            if (!l_Add->isAlive())
                                continue;

                            ++l_Count;
                        }

                        m_IntroDone = l_Count == 0;
                    });

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (m_IntroDone)
                        {
                            DoAction(eThogarActions::IntroEnd);

                            if (GameObject* l_IronGate = GameObject::GetGameObject(*me, m_Instance->GetData64(eFoundryGameObjects::IronGate)))
                                l_IronGate->SetGoState(GOState::GO_STATE_ACTIVE);
                        }
                    });
                }

                m_TrainID = eThogarTrains::FightTrainBeginning;

                if (m_Instance != nullptr)
                {
                    for (uint32 l_Entry = eFoundryGameObjects::MassiveDoorTrack4Right; l_Entry != eFoundryGameObjects::MassiveDoorTrack1Left; ++l_Entry)
                    {
                        if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(l_Entry)))
                            l_Door->SetGoState(GOState::GO_STATE_READY);
                    }
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID) override
            {
                switch (p_ID)
                {
                    case eGuidTypes::IntroAdd:
                    {
                        m_IntroTrashes.insert(p_Guid);
                        break;
                    }
                    case eGuidTypes::IntroRemove:
                    {
                        m_IntroTrashes.erase(p_Guid);

                        /// Intro finished!
                        if (!m_IntroTrashes.empty() || m_Instance == nullptr)
                            break;

                        switch (m_IntroState)
                        {
                            case eIntroStates::IronRaiders:
                            {
                                m_IntroState = eIntroStates::GunnerySergeants;

                                if (Creature* l_Wheels = me->FindNearestCreature(eThogarCreatures::TrainWheels, 100.0f))
                                {
                                    if (l_Wheels->IsAIEnabled)
                                        l_Wheels->AI()->DoAction(eThogarActions::TrainMoveEnd);
                                }

                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    SummonTrain(me, eThogarTrains::IntroSiegeTrain, eThogarActions::IntroBeginPart2, false);
                                });

                                break;
                            }
                            case eIntroStates::GunnerySergeants:
                            {
                                m_IntroState = eIntroStates::IntroEnd;

                                if (Creature* l_Wheels = me->FindNearestCreature(eThogarCreatures::TrainWheels, 100.0f))
                                {
                                    if (l_Wheels->IsAIEnabled)
                                        l_Wheels->AI()->DoAction(eThogarActions::TrainMoveEndPart2);
                                }

                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    SummonTrain(me, eThogarTrains::IntroWoodTrain, eThogarActions::IntroBeginPart3, false);
                                });

                                break;
                            }
                            default:
                                break;
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (m_IntroState == eIntroStates::GunnerySergeants && !m_IntroDone)
                {
                    if (p_Summon->GetEntry() == eThogarCreatures::IronGunnerySergeant ||
                        p_Summon->GetEntry() == eThogarCreatures::ManAtArmsIntro)
                        m_IntroTrashes.insert(p_Summon->GetGUID());
                }
                else
                    CreatureAI::JustSummoned(p_Summon);
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(eThogarTalks::TalkSlay);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                Talk(eThogarTalks::TalkAggro);

                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 1);

                m_Events.ScheduleEvent(eEvents::EventBerserker, eTimers::TimerBerserker);
                m_Events.ScheduleEvent(eEvents::EventEnkindle, eTimers::TimerEnkindle);
                m_Events.ScheduleEvent(eEvents::EventPrototypePulseGrenade, eTimers::TimerPulseGrenade);
                m_Events.ScheduleEvent(eEvents::EventSummonTrain, g_TrainDatas[m_TrainID].SpawnTimer);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->RemoveAllAreasTrigger();

                summons.DespawnAll();

                _JustDied();

                Talk(eThogarTalks::TalkDeath);

                if (m_Instance != nullptr)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEnkindle);

                    /// Allow loots and bonus loots to be enabled/disabled with a simple reload
                    if (sObjectMgr->IsDisabledEncounter(m_Instance->GetEncounterIDForBoss(me), GetDifficulty()))
                        me->SetLootRecipient(nullptr);
                    else
                        CastSpellToPlayers(me->GetMap(), me, eSpells::SpellThogarBonus, true);
                }
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                me->RemoveAllAreasTrigger();

                summons.DespawnAll();

                Talk(eThogarTalks::TalkWipe);

                if (m_Instance != nullptr)
                {
                    for (uint32 l_Entry = eFoundryGameObjects::MassiveDoorTrack4Right; l_Entry != eFoundryGameObjects::MassiveDoorTrack1Left; ++l_Entry)
                    {
                        if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(l_Entry)))
                            l_Door->SetGoState(GOState::GO_STATE_READY);
                    }

                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellEnkindle);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == eMove::MoveJump)
                {
                    m_IntroDone = true;

                    me->SetHomePosition(*me);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED | eUnitFlags::UNIT_FLAG_DISABLE_MOVE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eThogarActions::IntroEnd:
                    {
                        me->GetMotionMaster()->MoveJump(g_ThogarJumpPos, 30.0f, 10.0f, eMove::MoveJump);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo) override
            {
                if (p_Dest == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::SpellPrototypePulseGrenadeMissile)
                    me->CastSpell(p_Dest, eSpells::SpellPrototypePulseGrenadeAT, true);
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
                    case eEvents::EventBerserker:
                    {
                        me->CastSpell(me, eFoundrySpells::Berserker, true);
                        Talk(eThogarTalks::TalkBerserk);
                        break;
                    }
                    case eEvents::EventEnkindle:
                    {
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, eSpells::SpellEnkindle, false);

                        m_Events.ScheduleEvent(eEvents::EventEnkindle, eTimers::TimerEnkindle);
                        break;
                    }
                    case eEvents::EventPrototypePulseGrenade:
                    {
                        if (Player* l_Target = SelectRangedTarget(true))
                            me->CastSpell(l_Target, eSpells::SpellPrototypePulseGrenade, true);

                        m_Events.ScheduleEvent(eEvents::EventPrototypePulseGrenade, eTimers::TimerPulseGrenadeCD);
                        break;
                    }
                    case eEvents::EventSummonTrain:
                    {
                        if (m_TrainID >= g_TrainDatas.size())
                        {
                            printf("TrainID [%u] is too damn high! Max is %u!", m_TrainID, uint32(g_TrainDatas.size() - 1));
                            break;
                        }

                        SummonTrain(me, m_TrainID, eThogarActions::ActionNone);

                        ++m_TrainID;

                        uint32 l_Timer = g_TrainDatas[m_TrainID].SpawnTimer;
                        /// Few timers are set to 0, because of double spawns
                        m_Events.ScheduleEvent(eEvents::EventSummonTrain, l_Timer ? l_Timer - g_TrainDatas[m_TrainID - 1].SpawnTimer : l_Timer);
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
            return new boss_operator_thogarAI(p_Creature);
        }
};

/// Train - 76907
class npc_foundry_train_controller : public CreatureScript
{
    public:
        npc_foundry_train_controller() : CreatureScript("npc_foundry_train_controller") { }

        enum eMoves
        {
            MovementIntro = 5,
            MovementOuttro,
            SecondMovementIntro,
            SecondMovementOuttro,
            ThirdMovementIntro,
            ThirdMovementOuttro
        };

        enum eVisual
        {
            PreMovingTrain = 39795
        };

        struct npc_foundry_train_controllerAI : public ScriptedAI
        {
            npc_foundry_train_controllerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance  = p_Creature->GetInstanceScript();
                m_Vehicle   = p_Creature->GetVehicleKit();
                m_TrainID   = eThogarTrains::IntroTroopsTrain;
            }

            InstanceScript* m_Instance;
            Vehicle* m_Vehicle;

            uint64 m_SummonerGUID;

            uint8 m_TrainID;

            void Reset() override
            {
                m_SummonerGUID = 0;

                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
            }

            void DoAction(int32 const p_Action) override
            {
                if (m_Vehicle == nullptr || m_Instance == nullptr)
                    return;

                switch (p_Action)
                {
                    /// Action none, just move by default
                    case eThogarActions::ActionNone:
                    {
                        TrainDatas l_Datas = g_TrainDatas[m_TrainID];

                        switch (l_Datas.TrainType)
                        {
                            case eThogarMiscDatas::NonAddTrain:
                            {
                                StartTrain(0, false);

                                me->GetMotionMaster()->MovePoint(eMoves::ThirdMovementOuttro, l_Datas.RightToLeft ? g_TrainTrackEndPos[l_Datas.TrackID] : g_TrainTrackSpawnPos[l_Datas.TrackID], false);

                                HandleDoors(true);
                                break;
                            }
                            case eThogarMiscDatas::HalfLengthAddTrains:
                            case eThogarMiscDatas::FullLengthAddTrains:
                            case eThogarMiscDatas::SiegeTrain:
                            default:
                                break;
                        }

                        break;
                    }
                    /// Intro: Part1 - Move intro train
                    case eThogarActions::IntroBegin:
                    /// Intro: Part2 - Move intro train
                    case eThogarActions::IntroBeginPart2:
                    {
                        StartTrain(0, false);

                        me->GetMotionMaster()->MovePoint(p_Action == eThogarActions::IntroBegin ? eMoves::MovementIntro : eMoves::SecondMovementIntro, p_Action == eThogarActions::IntroBegin ? g_TrainTrackIntroEndPos : g_TrainTrackIntroSiegeEndPos, false);

                        HandleDoors(true);
                        break;
                    }
                    /// Intro: Part1 - Exit remaining passengers
                    case eThogarActions::IntroEnd:
                    {
                        if (Creature* l_Summoner = Creature::GetCreature(*me, m_SummonerGUID))
                            RemovePassengers(l_Summoner);

                        break;
                    }
                    /// Intro: Part1 - Move to end of the track
                    case eThogarActions::TrainMoveEnd:
                    {
                        for (int8 l_I = 0; l_I < MAX_VEHICLE_SEATS; ++l_I)
                        {
                            if (Unit* l_Passenger = m_Vehicle->GetPassenger(l_I))
                            {
                                /// Troop Transport visual
                                if (l_I == 1 || l_I == 3)
                                    l_Passenger->CastSpell(l_Passenger, eThogarSpells::TroopTransportClosed, true);

                                StartTrain(l_Passenger->GetGUID());
                            }
                        }

                        AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(eMoves::MovementOuttro, g_TrainTrackEndPos[g_TrainDatas[m_TrainID].TrackID]);
                        });

                        break;
                    }
                    /// Intro: Part2 - Move to end of the track
                    case eThogarActions::TrainMoveEndPart2:
                    {
                        StartTrain(0);

                        AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(eMoves::MovementOuttro, g_TrainTrackEndPos[g_TrainDatas[m_TrainID].TrackID]);
                        });

                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            HandleDoors(false);

                            if (GameObject* l_IronGate = GameObject::GetGameObject(*me, m_Instance->GetData64(eFoundryGameObjects::IronGate)))
                                l_IronGate->SetGoState(GOState::GO_STATE_ACTIVE);
                        });

                        break;
                    }
                    /// Intro: Part3 - Move wood train to boss
                    case eThogarActions::IntroBeginPart3:
                    {
                        StartTrain(0, false);

                        me->GetMotionMaster()->MovePoint(eMoves::ThirdMovementIntro, g_TrainTrackIntroWoodEndPos, false);

                        HandleDoors(true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetGUID(uint64 p_Guid, int32 /*p_ID*/) override
            {
                m_SummonerGUID = p_Guid;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                m_TrainID = p_Value;
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
            {
                if (m_Vehicle == nullptr && m_Instance == nullptr)
                    return;

                switch (p_ID)
                {
                    case eMoves::MovementIntro:
                    {
                        StopTrain();

                        if (Creature* l_Summoner = Creature::GetCreature(*me, m_SummonerGUID))
                            RemovePassengers(l_Summoner);

                        break;
                    }
                    case eMoves::MovementOuttro:
                    case eMoves::SecondMovementOuttro:
                    case eMoves::ThirdMovementOuttro:
                    {
                        if (p_ID != eMoves::MovementOuttro)
                            HandleDoors(false);

                        DespawnTrain();
                        break;
                    }
                    case eMoves::SecondMovementIntro:
                    {
                        StopTrain();

                        for (int8 l_I = 1; l_I < 3; ++l_I)
                        {
                            Unit* l_Passenger = m_Vehicle->GetPassenger(l_I);
                            if (l_Passenger == nullptr)
                                continue;

                            if (Vehicle* l_Train = l_Passenger->GetVehicleKit())
                            {
                                l_Passenger = l_Train->GetPassenger(0);
                                if (l_Passenger == nullptr)
                                    continue;

                                if (Vehicle* l_SiegeEngine = l_Passenger->GetVehicleKit())
                                {
                                    l_Passenger = l_SiegeEngine->GetPassenger(0);
                                    if (l_Passenger == nullptr)
                                        continue;

                                    if (Creature* l_Sergeant = l_Passenger->ToCreature())
                                    {
                                        l_Sergeant->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                        l_Sergeant->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                                        if (Player* l_Target = l_Sergeant->SelectNearestPlayerNotGM(60.0f))
                                        {
                                            if (l_Sergeant->IsAIEnabled)
                                                l_Sergeant->AI()->AttackStart(l_Target);
                                        }
                                    }
                                }

                                l_Passenger = l_Train->GetPassenger(1);
                                if (l_Passenger == nullptr)
                                    continue;

                                l_Passenger->ExitVehicle();

                                uint64 l_Guid = l_Passenger->GetGUID();
                                AddTimedDelayedOperation(100, [this, l_Guid]() -> void
                                {
                                    if (Creature* l_ManAtArms = Creature::GetCreature(*me, l_Guid))
                                    {
                                        l_ManAtArms->GetMotionMaster()->MoveJump(g_ManAtArmsExitPos, 30.0f, 20.0f);

                                        l_ManAtArms->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                        l_ManAtArms->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
                                    }
                                });

                                AddTimedDelayedOperation(500, [this, l_Guid]() -> void
                                {
                                    if (Creature* l_ManAtArms = Creature::GetCreature(*me, l_Guid))
                                    {
                                        if (Player* l_Target = l_ManAtArms->SelectNearestPlayerNotGM(30.0f))
                                        {
                                            if (l_ManAtArms->IsAIEnabled)
                                                l_ManAtArms->AI()->AttackStart(l_Target);
                                        }
                                    }
                                });
                            }
                        }

                        break;
                    }
                    case eMoves::ThirdMovementIntro:
                    {
                        StopTrain();

                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (m_Instance != nullptr)
                            {
                                if (Creature* l_Thogar = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BossOperatorThogar)))
                                {
                                    if (l_Thogar->IsAIEnabled)
                                        l_Thogar->AI()->Talk(eThogarTalks::TalkIntro);
                                }
                            }
                        });

                        StartTrain(0, true, 20 * TimeConstants::IN_MILLISECONDS);

                        AddTimedDelayedOperation(20 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (m_Instance != nullptr)
                            {
                                if (Creature* l_Thogar = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BossOperatorThogar)))
                                {
                                    /// Launch jump into room
                                    if (l_Thogar->IsAIEnabled)
                                        l_Thogar->AI()->DoAction(eThogarActions::IntroEnd);
                                }
                            }
                        });

                        AddTimedDelayedOperation(22 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            /// Launch move
                            me->GetMotionMaster()->MovePoint(eMoves::ThirdMovementOuttro, g_TrainTrackEndPos[g_TrainDatas[m_TrainID].TrackID]);

                            HandleDoors(true);
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
            }

            void RemovePassengers(Creature* p_Source)
            {
                Position const l_SourcePos = p_Source->GetHomePosition();
                bool l_IsLeft = l_SourcePos.IsNearPosition(&g_GromkarManAtArmsIntroLeftPos, 5.0f);

                if (Unit* l_TroopTransport = m_Vehicle->GetPassenger(l_IsLeft ? 1 : 3))
                {
                    l_TroopTransport->CastSpell(l_TroopTransport, eThogarSpells::TroopTransportOpen, true);

                    if (Vehicle* l_Transport = l_TroopTransport->GetVehicleKit())
                    {
                        for (int8 l_I = 0; l_I < (int8)eThogarMiscDatas::MaxIronRaiders; ++l_I)
                        {
                            if (!l_Transport->GetPassenger(l_I) || l_Transport->GetPassenger(l_I)->GetTypeId() != TypeID::TYPEID_UNIT)
                                continue;

                            if (Creature* l_Passenger = l_Transport->GetPassenger(l_I)->ToCreature())
                            {
                                l_Passenger->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                l_Passenger->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);

                                l_Passenger->ExitVehicle();

                                uint64 l_Guid = l_Passenger->GetGUID();
                                AddTimedDelayedOperation(100, [this, l_Guid, l_IsLeft, l_I]() -> void
                                {
                                    if (Creature* l_Passenger = Creature::GetCreature(*me, l_Guid))
                                        l_Passenger->GetMotionMaster()->MoveJump(l_IsLeft ? g_IronRaiderLeftExitPos[l_I] : g_IronRaiderRightExitPos[l_I], 30.0f, 10.0f);
                                });

                                AddTimedDelayedOperation(500, [this, l_Guid]() -> void
                                {
                                    if (Creature* l_Passenger = Creature::GetCreature(*me, l_Guid))
                                    {
                                        if (Unit* l_Victim = l_Passenger->SelectNearestPlayerNotGM(30.0f))
                                        {
                                            if (l_Passenger->IsAIEnabled)
                                                l_Passenger->AI()->AttackStart(l_Victim);
                                        }
                                    }
                                });
                            }
                        }
                    }
                }
            }

            void DespawnPassenger(Vehicle* p_Vehicle)
            {
                for (int8 l_I = 0; l_I < MAX_VEHICLE_SEATS; ++l_I)
                {
                    if (Unit* l_Passenger = p_Vehicle->GetPassenger(l_I))
                    {
                        if (Vehicle* l_Vehicle = l_Passenger->GetVehicleKit())
                            DespawnPassenger(l_Vehicle);

                        l_Passenger->ToCreature()->DespawnOrUnsummon();
                    }
                }
            }

            void DespawnTrain()
            {
                if (m_Vehicle == nullptr)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                DespawnPassenger(m_Vehicle);
            }

            /// Handle visuals for starting
            void StartTrain(uint64 p_Guid, bool p_Delayed = true, uint32 p_Delay = 1 * TimeConstants::IN_MILLISECONDS)
            {
                if (p_Guid)
                {
                    AddTimedDelayedOperation(p_Delayed ? p_Delay : 0, [this, p_Guid]() -> void
                    {
                        if (Unit* l_Passenger = Unit::GetUnit(*me, p_Guid))
                            l_Passenger->SendPlaySpellVisualKit(eVisual::PreMovingTrain, 0);
                    });

                    p_Delay += 2 * TimeConstants::IN_MILLISECONDS;

                    AddTimedDelayedOperation(p_Delayed ? p_Delay : 0, [this, p_Guid]() -> void
                    {
                        if (Unit* l_Passenger = Unit::GetUnit(*me, p_Guid))
                        {
                            l_Passenger->RemoveAura(eThogarSpells::StoppedFrontAura);

                            l_Passenger->CastSpell(l_Passenger, eThogarSpells::MovingTrain, true);
                            l_Passenger->CastSpell(l_Passenger, eThogarSpells::MovingFrontAura, true);
                        }
                    });
                }
                else
                {
                    for (int8 l_I = 0; l_I < MAX_VEHICLE_SEATS; ++l_I)
                    {
                        if (Unit* l_Passenger = m_Vehicle->GetPassenger(l_I))
                            StartTrain(l_Passenger->GetGUID(), p_Delayed, p_Delay);
                    }
                }
            }

            /// Handle visuals for stopping
            void StopTrain()
            {
                /// Update visuals
                for (int8 l_I = 0; l_I < MAX_VEHICLE_SEATS; ++l_I)
                {
                    if (Unit* l_Passenger = m_Vehicle->GetPassenger(l_I))
                    {
                        l_Passenger->RemoveAura(eThogarSpells::MovingFrontAura);
                        l_Passenger->RemoveAura(eThogarSpells::MovingTrain);

                        if (AreaTrigger* l_AreaTrigger = l_Passenger->GetAreaTrigger(eThogarSpells::MovingTrain))
                            l_AreaTrigger->SetDuration(1);

                        l_Passenger->CastSpell(l_Passenger, eThogarSpells::StoppedFrontAura, true);
                    }
                }
            }

            void HandleDoors(bool p_Apply)
            {
                if (GameObject* l_RightDoor = GameObject::GetGameObject(*me, m_Instance->GetData64(g_TrackDoors[g_TrainDatas[m_TrainID].TrackID].RightDoor)))
                    l_RightDoor->SetGoState(p_Apply ? GOState::GO_STATE_ACTIVE : GOState::GO_STATE_READY);

                if (GameObject* l_LeftDoor = GameObject::GetGameObject(*me, m_Instance->GetData64(g_TrackDoors[g_TrainDatas[m_TrainID].TrackID].LeftDoor)))
                    l_LeftDoor->SetGoState(p_Apply ? GOState::GO_STATE_ACTIVE : GOState::GO_STATE_READY);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_train_controllerAI(p_Creature);
        }
};

/// Iron Gunnery Sergeant - 81318
class npc_foundry_iron_gunnery_sergeant : public CreatureScript
{
    public:
        npc_foundry_iron_gunnery_sergeant() : CreatureScript("npc_foundry_iron_gunnery_sergeant") { }

        enum eSpells
        {
            DelayedSiegeBombSearcher    = 159480,
            DelayedSiegeBombChannel     = 159481,
            DelayedSiegeBombVisual      = 156489
        };

        enum eEvent
        {
            EventDelayedSiegeBomb = 1
        };

        enum eVisual
        {
            SiegeEngineVisual = 42258
        };

        struct npc_foundry_iron_gunnery_sergeantAI : public ScriptedAI
        {
            npc_foundry_iron_gunnery_sergeantAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            EventMap m_Events;

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanBeTargetedOutOfLOS() override
            {
                return true;
            }

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvent::EventDelayedSiegeBomb, 1 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance != nullptr)
                {
                    if (Creature* l_Thogar = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BossOperatorThogar)))
                    {
                        if (l_Thogar->IsAIEnabled)
                            l_Thogar->AI()->SetGUID(me->GetGUID(), 1);
                    }
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    if (Vehicle* l_SiegeEngine = me->GetVehicle())
                    {
                        if (l_SiegeEngine->GetBase()->GetTypeId() == TypeID::TYPEID_UNIT)
                            l_SiegeEngine->GetBase()->ToCreature()->DespawnOrUnsummon();
                    }

                    me->GetMotionMaster()->MoveFall();
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::DelayedSiegeBombSearcher)
                {
                    if (Unit* l_SiegeEngine = me->GetVehicleBase())
                    {
                        l_SiegeEngine->SetFacingTo(me->GetAngle(p_Target));
                        l_SiegeEngine->SendPlaySpellVisualKit(eVisual::SiegeEngineVisual, 0);
                        l_SiegeEngine->CastSpell(p_Target, eSpells::DelayedSiegeBombChannel, false);
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
                    case eEvent::EventDelayedSiegeBomb:
                    {
                        me->CastSpell(me, eSpells::DelayedSiegeBombSearcher, true);
                        m_Events.ScheduleEvent(eEvent::EventDelayedSiegeBomb, 11 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_iron_gunnery_sergeantAI(p_Creature);
        }
};

/// Siege Engine - 78982
/// Siege Engine - 81316
class npc_foundry_siege_engine : public CreatureScript
{
    public:
        npc_foundry_siege_engine() : CreatureScript("npc_foundry_siege_engine") { }

        enum eSpells
        {
            DelayedSiegeBombAoE     = 158084,
            DelayedSiegeBombMissile = 162286
        };

        struct npc_foundry_siege_engineAI : public ScriptedAI
        {
            npc_foundry_siege_engineAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                uint64 l_Guid = p_Summoner->GetGUID();
                AddTimedDelayedOperation(50, [this, l_Guid]() -> void
                {
                    if (Unit* l_Summoner = Creature::GetCreature(*me, l_Guid))
                    {
                        if (Creature* l_Sergeant = l_Summoner->SummonCreature(eThogarCreatures::IronGunnerySergeant, *me))
                        {
                            ApplyPassengerFlags(l_Sergeant, false);
                            l_Sergeant->EnterVehicle(me, 0);
                        }
                    }
                });
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo) override
            {
                if (p_Dest == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::DelayedSiegeBombAoE)
                {
                    if (InstanceScript* l_InstanceScript = me->GetInstanceScript())
                    {
                        if (Creature* l_Thogar = Creature::GetCreature(*me, l_InstanceScript->GetData64(eFoundryCreatures::BossOperatorThogar)))
                            l_Thogar->CastSpell(p_Dest, eSpells::DelayedSiegeBombMissile, true);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_siege_engineAI(p_Creature);
        }
};

/// Delayed Siege Bomb (periodic) - 159481
class spell_foundry_delayed_siege_bomb_periodic : public SpellScriptLoader
{
    public:
        spell_foundry_delayed_siege_bomb_periodic() : SpellScriptLoader("spell_foundry_delayed_siege_bomb_periodic") { }

        class spell_foundry_delayed_siege_bomb_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_delayed_siege_bomb_periodic_AuraScript)

            enum eSpell
            {
                DelayedSiegeBombMissile = 159482
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (Unit* l_Caster = GetCaster())
                        l_Caster->CastSpell(l_Target, eSpell::DelayedSiegeBombMissile, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_foundry_delayed_siege_bomb_periodic_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_delayed_siege_bomb_periodic_AuraScript();
        }
};

/// Prototype Pulse Grenade - 165194
class areatrigger_foundry_prototype_pulse_grenade : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_prototype_pulse_grenade() : AreaTriggerEntityScript("areatrigger_foundry_prototype_pulse_grenade") { }

        enum eSpell
        {
            PrototypePulseGrenadeDoT = 165195
        };

        std::set<uint64> m_AffectedPlayers;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 5.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                std::set<uint64> l_Targets;

                for (Unit* l_Iter : l_TargetList)
                {
                    l_Targets.insert(l_Iter->GetGUID());

                    if (!l_Iter->HasAura(eSpell::PrototypePulseGrenadeDoT, l_Caster->GetGUID()))
                    {
                        m_AffectedPlayers.insert(l_Iter->GetGUID());
                        l_Caster->CastSpell(l_Iter, eSpell::PrototypePulseGrenadeDoT, true);

                        if (l_Iter->IsPlayer())
                            l_Iter->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), true, *p_AreaTrigger, 0.7f, 1);
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
                        l_Unit->RemoveAura(eSpell::PrototypePulseGrenadeDoT, l_Caster->GetGUID());

                        if (l_Unit->IsPlayer())
                            l_Unit->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), false, Position());

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
                    {
                        l_Unit->RemoveAura(eSpell::PrototypePulseGrenadeDoT, l_Caster->GetGUID());

                        if (l_Unit->IsPlayer())
                            l_Unit->ToPlayer()->SendApplyMovementForce(p_AreaTrigger->GetGUID(), false, Position());
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_prototype_pulse_grenade();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_operator_thogar()
{
    /// Boss
    new boss_operator_thogar();

    /// Creatures
    new npc_foundry_train_controller();
    new npc_foundry_iron_gunnery_sergeant();
    new npc_foundry_siege_engine();

    /// Spells
    new spell_foundry_delayed_siege_bomb_periodic();

    /// AreaTriggers
    new areatrigger_foundry_prototype_pulse_grenade();
}
#endif
