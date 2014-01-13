/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Scripts for spells with SPELLFAMILY_DEATHKNIGHT and SPELLFAMILY_GENERIC spells used by deathknight players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dk_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum DeathKnightSpells
{
    SPELL_DK_ANTI_MAGIC_SHELL_TALENT            = 51052,
    SPELL_DK_BLACK_ICE_R1                       = 49140,
    SPELL_DK_BLOOD_BOIL_TRIGGERED               = 65658,
    SPELL_DK_BLOOD_GORGED_HEAL                  = 50454,
    SPELL_DK_CORPSE_EXPLOSION_TRIGGERED         = 43999,
    SPELL_DK_CORPSE_EXPLOSION_VISUAL            = 51270,
	SPELL_DK_DEATH_COIL							= 47541,
    SPELL_DK_DEATH_COIL_DAMAGE                  = 47632,
    SPELL_DK_DEATH_COIL_HEAL                    = 47633,
    SPELL_DK_DEATH_STRIKE_HEAL                  = 45470,
	SPELL_DK_BLOOD_SHIELD						= 77535,
	SPELL_DK_BLOOD_MASTERY						= 77513,
	SPELL_DK_BLOOD_RITES						= 50034,
	SPELL_DK_SCENT_OF_BLOOD						= 50421,
    SPELL_DK_GHOUL_EXPLODE                      = 47496,
    SPELL_DK_GLYPH_OF_ICEBOUND_FORTITUDE        = 58625,
    SPELL_DK_RUNIC_POWER_ENERGIZE               = 49088,
    SPELL_DK_SCOURGE_STRIKE_TRIGGERED           = 70890,
    SPELL_DK_WILL_OF_THE_NECROPOLIS_TALENT_R1   = 49189,
    SPELL_DK_WILL_OF_THE_NECROPOLIS_AURA_R1     = 52284,
    SPELL_DK_BLOOD_PRESENCE                     = 48266,
    SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED  = 63611,
    SPELL_DK_UNHOLY_PRESENCE                    = 48265,
    SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED = 63622,
    SPELL_DK_ITEM_SIGIL_VENGEFUL_HEART          = 64962,
    SPELL_DK_ITEM_T8_MELEE_4P_BONUS             = 64736,
	SPELL_DK_ICY_TOUCH							= 45477,
	SPELL_DK_SOUL_REAPER						= 130735,
    SPELL_DK_SOUL_REAPER_HASTE                  = 114868,
    SPELL_DK_SOUL_REAPER_DAMAGE                 = 114867,
	SPELL_DK_DEATH_AND_DECAY					= 52212,
	SPELL_DK_REMORSELESS_WINTER					= 115000,
	SPELL_DK_REMORSELESS_WINTER_STUN			= 115001,
	SPELL_DK_FROST_FEVER                        = 55095,
};

enum DeathKnightSpellIcons
{
    DK_ICON_ID_IMPROVED_DEATH_STRIKE            = 2751
};

// 50462 - Anti-Magic Shell (on raid member)
class spell_dk_anti_magic_shell_raid : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_raid() : SpellScriptLoader("spell_dk_anti_magic_shell_raid") { }

        class spell_dk_anti_magic_shell_raid_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_raid_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // TODO: this should absorb limited amount of damage, but no info on calculation formula
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_raid_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_raid_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_shell_raid_AuraScript();
        }
};

// 48707 - Anti-Magic Shell (on self)
class spell_dk_anti_magic_shell_self : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_self() : SpellScriptLoader("spell_dk_anti_magic_shell_self") { }

        class spell_dk_anti_magic_shell_self_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_self_AuraScript);

            uint32 absorbPct, hpPct;
            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                hpPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_RUNIC_POWER_ENERGIZE))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                amount = GetCaster()->CountPctFromMaxHealth(hpPct);
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                absorbAmount = std::min(CalculatePct(dmgInfo.GetDamage(), absorbPct), GetTarget()->CountPctFromMaxHealth(hpPct));
            }

            void Trigger(AuraEffect* aurEff, DamageInfo & /*dmgInfo*/, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                // damage absorbed by Anti-Magic Shell energizes the DK with additional runic power.
                // This, if I'm not mistaken, shows that we get back ~20% of the absorbed damage as runic power.
                int32 bp = absorbAmount * 2 / 10;
                target->CastCustomSpell(target, SPELL_DK_RUNIC_POWER_ENERGIZE, &bp, NULL, NULL, true, NULL, aurEff);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_self_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Absorb, EFFECT_0);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_shell_self_AuraScript();
        }
};

// 50461 - Anti-Magic Zone
class spell_dk_anti_magic_zone : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_zone() : SpellScriptLoader("spell_dk_anti_magic_zone") { }

        class spell_dk_anti_magic_zone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_zone_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_ANTI_MAGIC_SHELL_TALENT))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                SpellInfo const* talentSpell = sSpellMgr->GetSpellInfo(SPELL_DK_ANTI_MAGIC_SHELL_TALENT);
                amount = talentSpell->Effects[EFFECT_0].CalcValue(GetCaster());
                if (Player* player = GetCaster()->ToPlayer())
                     amount += int32(2 * player->GetTotalAttackPowerValue(BASE_ATTACK));
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                 absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_zone_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_zone_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_zone_AuraScript();
        }
};

// 48721 - Blood Boil
class spell_dk_blood_boil : public SpellScriptLoader
{
    public:
        spell_dk_blood_boil() : SpellScriptLoader("spell_dk_blood_boil") { }

        class spell_dk_blood_boil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_blood_boil_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_BLOOD_BOIL_TRIGGERED))
                    return false;
                return true;
            }

            bool Load()
            {
                _executed = false;
                return GetCaster()->GetTypeId() == TYPEID_PLAYER && GetCaster()->getClass() == CLASS_DEATH_KNIGHT;
            }

            void HandleAfterHit()
            {
                if (_executed || !GetHitUnit())
                    return;

                _executed = true;
                GetCaster()->CastSpell(GetCaster(), SPELL_DK_BLOOD_BOIL_TRIGGERED, true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_dk_blood_boil_SpellScript::HandleAfterHit);
            }

            bool _executed;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_blood_boil_SpellScript();
        }
};

// 50453 - Bloodworms Health Leech
class spell_dk_blood_gorged : public SpellScriptLoader
{
    public:
        spell_dk_blood_gorged() : SpellScriptLoader("spell_dk_blood_gorged") { }

        class spell_dk_blood_gorged_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_blood_gorged_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_BLOOD_GORGED_HEAL))
                    return false;
                return true;
            }

            bool Load()
            {
                _procTarget = NULL;
                return true;
            }

            bool CheckProc(ProcEventInfo& /*eventInfo*/)
            {
                _procTarget = GetTarget()->GetOwner();
                return _procTarget;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                int32 bp = int32(eventInfo.GetDamageInfo()->GetDamage() * 1.5f);
                GetTarget()->CastCustomSpell(SPELL_DK_BLOOD_GORGED_HEAL, SPELLVALUE_BASE_POINT0, bp, _procTarget, true, NULL, aurEff);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_dk_blood_gorged_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_dk_blood_gorged_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }

        private:
            Unit* _procTarget;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_blood_gorged_AuraScript();
        }
};

// 49158 - Corpse Explosion (51325, 51326, 51327, 51328)
class spell_dk_corpse_explosion : public SpellScriptLoader
{
    public:
        spell_dk_corpse_explosion() : SpellScriptLoader("spell_dk_corpse_explosion") { }

        class spell_dk_corpse_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_corpse_explosion_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_CORPSE_EXPLOSION_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_DK_GHOUL_EXPLODE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_CORPSE_EXPLOSION_VISUAL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    int32 bp = 0;
                    if (unitTarget->isAlive())  // Living ghoul as a target
                    {
                        bp = int32(unitTarget->CountPctFromMaxHealth(25));
                        unitTarget->CastCustomSpell(unitTarget, SPELL_DK_GHOUL_EXPLODE, &bp, NULL, NULL, false);
                    }
                    else                        // Some corpse
                    {
                        bp = GetEffectValue();
                        GetCaster()->CastCustomSpell(unitTarget, GetSpellInfo()->Effects[EFFECT_1].CalcValue(), &bp, NULL, NULL, true);
                        // Corpse Explosion (Suicide)
                        unitTarget->CastSpell(unitTarget, SPELL_DK_CORPSE_EXPLOSION_TRIGGERED, true);
                    }
                    // Set corpse look
                    GetCaster()->CastSpell(unitTarget, SPELL_DK_CORPSE_EXPLOSION_VISUAL, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_corpse_explosion_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_corpse_explosion_SpellScript();
        }
};

// 47541 - Death Coil
class spell_dk_death_coil : public SpellScriptLoader
{
    public:
        spell_dk_death_coil() : SpellScriptLoader("spell_dk_death_coil") { }

        class spell_dk_death_coil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_coil_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_COIL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    if (caster->IsFriendlyTo(target))
						caster->CastSpell(target, SPELL_DK_DEATH_COIL_HEAL);
                    else caster->CastSpell(target, SPELL_DK_DEATH_COIL_DAMAGE);
                }
            }

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetExplTargetUnit())
                {
                    if (!caster->IsFriendlyTo(target) && !caster->isInFront(target))
                        return SPELL_FAILED_UNIT_NOT_INFRONT;

                    if (target->IsFriendlyTo(caster) && target->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_coil_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_coil_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_coil_SpellScript();
        }
};

// 47632 - Death Coil damage
class spell_dk_death_coil_damage : public SpellScriptLoader
{
    public:
        spell_dk_death_coil_damage() : SpellScriptLoader("spell_dk_death_coil_damage") { }

        class spell_dk_death_coil_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_coil_damage_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_COIL_DAMAGE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster()->ToPlayer();
				uint32 damage = GetHitDamage();
                
				SetHitDamage(damage + caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.5f);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_coil_damage_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_coil_damage_SpellScript();
        }
};

// 47633 - Death Coil heal
class spell_dk_death_coil_heal : public SpellScriptLoader
{
    public:
        spell_dk_death_coil_heal() : SpellScriptLoader("spell_dk_death_coil_heal") { }

        class spell_dk_death_coil_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_coil_heal_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_COIL_HEAL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster()->ToPlayer();
				uint32 healAmount = GetHitHeal();
                
				SetHitHeal(healAmount + caster->GetTotalAttackPowerValue(BASE_ATTACK) * 1.8f);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_coil_heal_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_coil_heal_SpellScript();
        }
};

// 52751 - Death Gate
class spell_dk_death_gate : public SpellScriptLoader
{
    public:
        spell_dk_death_gate() : SpellScriptLoader("spell_dk_death_gate") {}

        class spell_dk_death_gate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_gate_SpellScript);

            SpellCastResult CheckClass()
            {
                if (GetCaster()->getClass() != CLASS_DEATH_KNIGHT)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_DEATH_KNIGHT);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, GetEffectValue(), false);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_SpellScript::CheckClass);
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_gate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_gate_SpellScript();
        }
};

// 49560 - Death Grip
class spell_dk_death_grip : public SpellScriptLoader
{
    public:
        spell_dk_death_grip() : SpellScriptLoader("spell_dk_death_grip") { }

        class spell_dk_death_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_grip_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                Position const* pos = GetExplTargetDest();
                if (Unit* target = GetHitUnit())
                {
                    if (!target->HasAuraType(SPELL_AURA_DEFLECT_SPELLS)) // Deterrence
                        target->CastSpell(pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), damage, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_grip_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_grip_SpellScript();
        }
};

// 48743 - Death Pact
class spell_dk_death_pact : public SpellScriptLoader
{
    public:
        spell_dk_death_pact() : SpellScriptLoader("spell_dk_death_pact") { }

        class spell_dk_death_pact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_pact_SpellScript);

            SpellCastResult CheckCast()
            {
                // Check if we have valid targets, otherwise skip spell casting here
                if (Player* player = GetCaster()->ToPlayer())
                    for (Unit::ControlList::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                        if (Creature* undeadPet = (*itr)->ToCreature())
                            if (undeadPet->isAlive() &&
                                undeadPet->GetOwnerGUID() == player->GetGUID() &&
                                undeadPet->GetCreatureType() == CREATURE_TYPE_UNDEAD &&
                                undeadPet->IsWithinDist(player, 100.0f, false))
                                return SPELL_CAST_OK;

                return SPELL_FAILED_NO_PET;
            }

            void FilterTargets(std::list<WorldObject*>& targetList)
            {
                Unit* target = NULL;
                for (std::list<WorldObject*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                {
                    if (Unit* unit = (*itr)->ToUnit())
                        if (unit->GetOwnerGUID() == GetCaster()->GetGUID() && unit->GetCreatureType() == CREATURE_TYPE_UNDEAD)
                        {
                            target = unit;
                            break;
                        }
                }

                targetList.clear();
                if (target)
                    targetList.push_back(target);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_pact_SpellScript::CheckCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_death_pact_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_pact_SpellScript();
        }
};

// -49998 - Death Strike
class spell_dk_death_strike : public SpellScriptLoader
{
    public:
        spell_dk_death_strike() : SpellScriptLoader("spell_dk_death_strike") { }

        class spell_dk_death_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_strike_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_STRIKE_HEAL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if(Player* caster = GetCaster()->ToPlayer())
                {
                    int32 bp = (int32) (0.2f * caster->GetDamageTakenInPastSecs(5, false, true));
					if(bp < (caster->GetMaxHealth() * 0.07f))
						bp = (int32)(caster->GetMaxHealth() * 0.07f);

					// Scent of Blood
					if (Aura* scentOfBlood = caster->GetAura(SPELL_DK_SCENT_OF_BLOOD))
                    {
                        bp += bp * 0.2f * scentOfBlood->GetStackAmount();
						caster->RemoveAura(SPELL_DK_SCENT_OF_BLOOD);
                    }

                    caster->CastCustomSpell(caster, SPELL_DK_DEATH_STRIKE_HEAL, &bp, NULL, NULL, false);

					// Blood Shield - Mastery (Blood)
					if(caster->HasAura(SPELL_DK_BLOOD_MASTERY))
					{
						int32 mbp = 0.5f * bp;
						if(caster->HasAura(SPELL_DK_BLOOD_SHIELD))
						{
							
							
							if (Aura* bloodShield = caster->GetAura(SPELL_DK_BLOOD_SHIELD))
							{
								mbp = bloodShield->GetEffect(0)->GetAmount() + mbp;
								if(mbp > caster->GetMaxHealth())
									mbp = caster->GetMaxHealth();

								bloodShield->GetEffect(0)->ChangeAmount(mbp);
							}
							else
								caster->CastCustomSpell(caster, SPELL_DK_BLOOD_SHIELD, &mbp, NULL, NULL, false);
							
						}
						else
							caster->CastCustomSpell(caster, SPELL_DK_BLOOD_SHIELD, &mbp, NULL, NULL, false);
					}

                }
            }

			void HandleAfterHit()
			{
				if(Player* caster = GetCaster()->ToPlayer())
                {
					// Blood Rite
					/*
					if(caster->HasAura(SPELL_DK_BLOOD_RITES))
					{
						for(uint8 i = 2; i<6; ++i)
						{
							caster->ConvertRune(i, RUNE_DEATH);
							caster->SetRuneConvertAura(i, NULL);
						}
					}
					*/
				}
			}

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
				AfterHit += SpellHitFn(spell_dk_death_strike_SpellScript::HandleAfterHit);
            }

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_strike_SpellScript();
        }
};

// 47496 - Explode, Ghoul spell for Corpse Explosion
class spell_dk_ghoul_explode : public SpellScriptLoader
{
    public:
        spell_dk_ghoul_explode() : SpellScriptLoader("spell_dk_ghoul_explode") { }

        class spell_dk_ghoul_explode_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_ghoul_explode_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_CORPSE_EXPLOSION_TRIGGERED))
                    return false;
                return true;
            }

            void Suicide(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    // Corpse Explosion (Suicide)
                    unitTarget->CastSpell(unitTarget, SPELL_DK_CORPSE_EXPLOSION_TRIGGERED, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_ghoul_explode_SpellScript::Suicide, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_ghoul_explode_SpellScript();
        }
};

// 48792 - Icebound Fortitude
class spell_dk_icebound_fortitude : public SpellScriptLoader
{
    public:
        spell_dk_icebound_fortitude() : SpellScriptLoader("spell_dk_icebound_fortitude") { }

        class spell_dk_icebound_fortitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_icebound_fortitude_AuraScript);

            bool Load()
            {
                Unit* caster = GetCaster();
                return caster && caster->GetTypeId() == TYPEID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 value = amount;
                    uint32 defValue = uint32(caster->ToPlayer()->GetSkillValue(SKILL_DEFENSE) + caster->ToPlayer()->GetRatingBonusValue(CR_DEFENSE_SKILL));

                    if (defValue > 400)
                        value -= int32((defValue - 400) * 0.15);

                    // Glyph of Icebound Fortitude
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_DK_GLYPH_OF_ICEBOUND_FORTITUDE, EFFECT_0))
                    {
                        int32 valMax = -aurEff->GetAmount();
                        if (value > valMax)
                            value = valMax;
                    }
                    amount = value;
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_icebound_fortitude_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_icebound_fortitude_AuraScript();
        }
};

// 50365, 50371 - Improved Blood Presence
class spell_dk_improved_blood_presence : public SpellScriptLoader
{
    public:
        spell_dk_improved_blood_presence() : SpellScriptLoader("spell_dk_improved_blood_presence") { }

        class spell_dk_improved_blood_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_improved_blood_presence_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_BLOOD_PRESENCE) || !sSpellMgr->GetSpellInfo(SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED))
                    return false;
                return true;
            }

            void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (!target->HasAura(SPELL_DK_BLOOD_PRESENCE) && !target->HasAura(SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED))
                {
                    int32 basePoints1 = aurEff->GetAmount();
                    target->CastCustomSpell(target, SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED, NULL, &basePoints1, NULL, true, 0, aurEff);
                }
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (!target->HasAura(SPELL_DK_BLOOD_PRESENCE))
                    target->RemoveAura(SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_improved_blood_presence_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_improved_blood_presence_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_improved_blood_presence_AuraScript();
        }
};

// 50391, 50392 - Improved Unholy Presence
class spell_dk_improved_unholy_presence : public SpellScriptLoader
{
    public:
        spell_dk_improved_unholy_presence() : SpellScriptLoader("spell_dk_improved_unholy_presence") { }

        class spell_dk_improved_unholy_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_improved_unholy_presence_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_UNHOLY_PRESENCE) || !sSpellMgr->GetSpellInfo(SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED))
                    return false;
                return true;
            }

            void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (target->HasAura(SPELL_DK_UNHOLY_PRESENCE) && !target->HasAura(SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED))
                {
                    // Not listed as any effect, only base points set in dbc
                    int32 basePoints0 = aurEff->GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                    target->CastCustomSpell(target, SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED, &basePoints0, &basePoints0, &basePoints0, true, 0, aurEff);
                }
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAura(SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_improved_unholy_presence_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_dk_improved_unholy_presence_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_improved_unholy_presence_AuraScript();
        }
};

// 59754 Rune Tap - Party
class spell_dk_rune_tap_party : public SpellScriptLoader
{
    public:
        spell_dk_rune_tap_party() : SpellScriptLoader("spell_dk_rune_tap_party") { }

        class spell_dk_rune_tap_party_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_rune_tap_party_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove(GetCaster());
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dk_rune_tap_party_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_PARTY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_rune_tap_party_SpellScript();
        }
};

// 55090 - Scourge Strike (55265, 55270, 55271)
class spell_dk_scourge_strike : public SpellScriptLoader
{
    public:
        spell_dk_scourge_strike() : SpellScriptLoader("spell_dk_scourge_strike") { }

        class spell_dk_scourge_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_scourge_strike_SpellScript);
            float multiplier;

            bool Load()
            {
                multiplier = 1.0f;
                return true;
            }

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_SCOURGE_STRIKE_TRIGGERED))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    multiplier = (GetEffectValue() * unitTarget->GetDiseasesByCaster(caster->GetGUID()) / 100.f);
                    // Death Knight T8 Melee 4P Bonus
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_DK_ITEM_T8_MELEE_4P_BONUS, EFFECT_0))
                        AddPct(multiplier, aurEff->GetAmount());
                }
            }

            void HandleAfterHit()
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    int32 bp = GetHitDamage() * multiplier;

                    if (AuraEffect* aurEff = caster->GetAuraEffectOfRankedSpell(SPELL_DK_BLACK_ICE_R1, EFFECT_0))
                        AddPct(bp, aurEff->GetAmount());

                    caster->CastCustomSpell(unitTarget, SPELL_DK_SCOURGE_STRIKE_TRIGGERED, &bp, NULL, NULL, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_scourge_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
                AfterHit += SpellHitFn(spell_dk_scourge_strike_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_scourge_strike_SpellScript();
        }
};

// 49145 - Spell Deflection
class spell_dk_spell_deflection : public SpellScriptLoader
{
    public:
        spell_dk_spell_deflection() : SpellScriptLoader("spell_dk_spell_deflection") { }

        class spell_dk_spell_deflection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_spell_deflection_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                // You have a chance equal to your Parry chance
                if ((dmgInfo.GetDamageType() == SPELL_DIRECT_DAMAGE) && roll_chance_f(GetTarget()->GetUnitParryChance()))
                    absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_spell_deflection_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_spell_deflection_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_spell_deflection_AuraScript();
        }
};

// 55233 - Vampiric Blood
class spell_dk_vampiric_blood : public SpellScriptLoader
{
    public:
        spell_dk_vampiric_blood() : SpellScriptLoader("spell_dk_vampiric_blood") { }

        class spell_dk_vampiric_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_vampiric_blood_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = GetUnitOwner()->CountPctFromMaxHealth(amount);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_vampiric_blood_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_HEALTH);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_vampiric_blood_AuraScript();
        }
};

// 52284 - Will of the Necropolis
class spell_dk_will_of_the_necropolis : public SpellScriptLoader
{
    public:
        spell_dk_will_of_the_necropolis() : SpellScriptLoader("spell_dk_will_of_the_necropolis") { }

        class spell_dk_will_of_the_necropolis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_will_of_the_necropolis_AuraScript);

            bool Validate(SpellInfo const* spellInfo)
            {
                // can't use other spell than will of the necropolis due to spell_ranks dependency
                if (sSpellMgr->GetFirstSpellInChain(SPELL_DK_WILL_OF_THE_NECROPOLIS_AURA_R1) != sSpellMgr->GetFirstSpellInChain(spellInfo->Id))
                    return false;

                uint8 rank = sSpellMgr->GetSpellRank(spellInfo->Id);
                if (!sSpellMgr->GetSpellWithRank(SPELL_DK_WILL_OF_THE_NECROPOLIS_TALENT_R1, rank, true))
                    return false;

                return true;
            }

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                // min pct of hp is stored in effect 0 of talent spell
                uint32 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
                SpellInfo const* talentProto = sSpellMgr->GetSpellInfo(sSpellMgr->GetSpellWithRank(SPELL_DK_WILL_OF_THE_NECROPOLIS_TALENT_R1, rank));

                int32 remainingHp = int32(GetTarget()->GetHealth() - dmgInfo.GetDamage());
                int32 minHp = int32(GetTarget()->CountPctFromMaxHealth(talentProto->Effects[EFFECT_0].CalcValue(GetCaster())));

                // Damage that would take you below [effect0] health or taken while you are at [effect0]
                if (remainingHp < minHp)
                    absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_will_of_the_necropolis_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_will_of_the_necropolis_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_will_of_the_necropolis_AuraScript();
        }
};

// 73975 - Necrotic Strike
class spell_dk_necrotic_strike : public SpellScriptLoader
{
    public:
        spell_dk_necrotic_strike() : SpellScriptLoader("spell_dk_necrotic_strike") { }

        class spell_dk_necrotic_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_necrotic_strike_AuraScript);

            bool Validate(SpellInfo const* spellInfo)
            {
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool &canBeRecalculated)
            {
				if(Player* caster = GetCaster()->ToPlayer())
				{
					amount = caster->GetTotalAttackPowerValue(BASE_ATTACK);
					canBeRecalculated = false;
				}
            }

			void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
			{
				if(Player* caster = GetCaster()->ToPlayer())
				{
					// Need to use 1 Death Rune
				}
			}

            void Register()
            {
				DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_necrotic_strike_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_dk_necrotic_strike_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_necrotic_strike_AuraScript();
        }
};

// 45477 : Icy Touch
class spell_dk_icy_touch : public SpellScriptLoader
{
    public:
        spell_dk_icy_touch() : SpellScriptLoader("spell_dk_icy_touch") { }

        class spell_dk_icy_touch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_icy_touch_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_ICY_TOUCH))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster()->ToPlayer();
				uint32 damage = GetHitDamage();

				SetHitDamage(damage + (caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.319f));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_icy_touch_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_icy_touch_SpellScript();
        }
};

// 49184 : Howling Blast
class spell_dk_howling_blast : public SpellScriptLoader
{
    public:
        spell_dk_howling_blast() : SpellScriptLoader("spell_dk_howling_blast") { }

        class spell_dk_howling_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_howling_blast_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_ICY_TOUCH))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster()->ToPlayer();
				Unit* target = GetHitUnit();
				uint32 damage = GetHitDamage();

				SetHitDamage(damage + (caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.681f));
				caster->CastSpell(target, SPELL_DK_FROST_FEVER, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_howling_blast_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_howling_blast_SpellScript();
        }
};

// Soul Reaper - 130736 (unholy) or 130735 (frost) or 114866 (blood)
class spell_dk_soul_reaper : public SpellScriptLoader
{
    public:
        spell_dk_soul_reaper() : SpellScriptLoader("spell_dk_soul_reaper") { }

        class spell_dk_soul_reaper_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_soul_reaper_AuraScript);

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                {
                    AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();

                    if (removeMode == AURA_REMOVE_BY_DEATH)
                        GetCaster()->CastSpell(GetCaster(), SPELL_DK_SOUL_REAPER_HASTE, true);
                    else if (removeMode == AURA_REMOVE_BY_EXPIRE && GetTarget()->GetHealthPct() < 35.0f)
                        GetCaster()->CastSpell(GetTarget(), SPELL_DK_SOUL_REAPER_DAMAGE, true);
                }
            }

            void Register()
            {
                OnEffectRemove += AuraEffectApplyFn(spell_dk_soul_reaper_AuraScript::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_soul_reaper_AuraScript();
        }
};

// 114867 : Soul Reaper damage (triggered)
class spell_dk_soul_reaper_damage : public SpellScriptLoader
{
    public:
        spell_dk_soul_reaper_damage() : SpellScriptLoader("spell_dk_soul_reaper_damage") { }

        class spell_dk_soul_reaper_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_soul_reaper_damage_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_SOUL_REAPER_DAMAGE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster()->ToPlayer();
				uint32 damage = GetHitDamage();

				SetHitDamage(damage + caster->GetTotalAttackPowerValue(BASE_ATTACK));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_soul_reaper_damage_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_soul_reaper_damage_SpellScript();
        }
};

// 52212 : Death and Decay
class spell_dk_death_and_decay : public SpellScriptLoader
{
    public:
        spell_dk_death_and_decay() : SpellScriptLoader("spell_dk_death_and_decay") { }

        class spell_dk_death_and_decay_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_and_decay_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_SOUL_REAPER_DAMAGE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster()->ToPlayer();
				uint32 damage = GetHitDamage();

				SetHitDamage(damage + (caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.064f));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_and_decay_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_and_decay_SpellScript();
        }
};


// 115000 : Remorseless Winter
class spell_dk_remorseless_winter : public SpellScriptLoader
{
    public:
        spell_dk_remorseless_winter() : SpellScriptLoader("spell_dk_remorseless_winter") { }

        class spell_dk_remorseless_winter_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_remorseless_winter_AuraScript);

            void HandleAfterEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if(Unit* target = GetTarget())
					if(Aura *aur = target->GetAura(SPELL_DK_REMORSELESS_WINTER))
						if((aur->GetStackAmount() == 5) && !target->HasAura(SPELL_DK_REMORSELESS_WINTER_STUN))
							if(GetCaster())
								GetCaster()->CastSpell(target, SPELL_DK_REMORSELESS_WINTER_STUN, true);
			}

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_dk_remorseless_winter_AuraScript::HandleAfterEffectApply, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_remorseless_winter_AuraScript();
        }
};

class spell_dk_raise_dead : public SpellScriptLoader
{
public :
    spell_dk_raise_dead() : SpellScriptLoader("spell_dk_raise_dead")
    {
        
    }
    
    class spell_dk_raise_dead_SpellScript : public SpellScript
    {
    private :
        PrepareSpellScript(spell_dk_raise_dead_SpellScript);
        
        bool Validate(const SpellInfo *spellInfo)
        {
            return true ;
        }
        
        bool Load()
        {
            return true ;
        }
        
        void HandleOnCast()
        {
            Unit* caster = GetCaster();
            if(caster)
            {
                Player* player = caster->ToPlayer();
                SpellInfo const* spellInfo = GetSpellInfo();
                if(player && spellInfo)
                {
                    sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Raise Dead: Summoning ; player spec %u", uint32(player->GetActiveSpec()));
                    player->CastSpell(player, spellInfo->Effects[(player->GetPrimaryTalentTree(player->GetActiveSpec()) == TALENT_TREE_DEATH_KNIGHT_UNHOLY) ? 1 : 0].BasePoints, true);
                }
            }
        }
        
        void Register()
        {
            OnCast += SpellCastFn(spell_dk_raise_dead_SpellScript::HandleOnCast);
        }
    };
    
    SpellScript* GetSpellScript() const
    {
        return new spell_dk_raise_dead_SpellScript();
    }
};

class spell_dk_conversion : public SpellScriptLoader
{
public :
    spell_dk_conversion() : SpellScriptLoader("spell_dk_conversion")
    {

    }

    class spell_dk_conversion_AuraScript : public AuraScript
    {
    private :
        PrepareAuraScript(spell_dk_conversion_AuraScript);

        bool Validate(const SpellInfo *spellInfo)
        {
            return true ;
        }

        bool Load()
        {
            return true ;
        }

        void HandlePeriodicTick(AuraEffect const* auraEff)
        {
            sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Conversion: Entering OnEffectPeriodic Handler");
            Unit* caster = auraEff->GetCaster();
            if(caster && auraEff->GetBase())
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS: Conversion (Periodic Handler): found a caster (guid %u, name %s), with runic power amount of %u", caster->GetGUID(), caster->GetName().c_str(), uint32(caster->GetPower(POWER_RUNIC_POWER)));
                if(caster->GetPower(POWER_RUNIC_POWER) >= 10)
                    caster->ModifyPower(POWER_RUNIC_POWER, -10);
                else
                    auraEff->GetBase()->Remove();
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_conversion_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_conversion_AuraScript();
    }
};

void AddSC_deathknight_spell_scripts()
{
    new spell_dk_anti_magic_shell_raid();
    new spell_dk_anti_magic_shell_self();
    new spell_dk_anti_magic_zone();
    new spell_dk_blood_boil();
    new spell_dk_blood_gorged();
    new spell_dk_corpse_explosion();
    new spell_dk_death_coil();
	new spell_dk_death_coil_damage();
	new spell_dk_death_coil_heal();
    new spell_dk_death_gate();
    new spell_dk_death_grip();
    new spell_dk_death_pact();
    new spell_dk_death_strike();
    new spell_dk_ghoul_explode();
    new spell_dk_icebound_fortitude();
    new spell_dk_improved_blood_presence();
    new spell_dk_improved_unholy_presence();
    new spell_dk_rune_tap_party();
    new spell_dk_scourge_strike();
    new spell_dk_spell_deflection();
    new spell_dk_vampiric_blood();
    new spell_dk_will_of_the_necropolis();
	new spell_dk_necrotic_strike();
	new spell_dk_icy_touch();
	new spell_dk_howling_blast();
	new spell_dk_soul_reaper();
	new spell_dk_soul_reaper_damage();
	new spell_dk_death_and_decay();
	new spell_dk_remorseless_winter();
	new spell_dk_raise_dead();
	new spell_dk_conversion();
}
