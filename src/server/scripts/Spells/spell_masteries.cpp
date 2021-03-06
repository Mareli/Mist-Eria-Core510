#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"

enum MasterySpells
{
    MASTERY_WARRIOR_ARMS                = 76838,
    MASTERY_WARRIOR_FURY                = 76856,
    MASTERY_WARRIOR_PROTECTION          = 76857,

    MASTERY_PALADIN_HOLY                = 76669,
    MASTERY_PALADIN_PROTECTION          = 76671,
    MASTERY_PALADIN_RETRIBUTION         = 76672,
    
    MASTERY_HUNTER_BEASTMASTER          = 76657,
    MASTERY_HUNTER_MARKSMAN             = 76659,
    MASTERY_HUNTER_SURVIVAL             = 76658,

    MASTERY_ROGUE_ASSASSINATION         = 76803,
    MASTERY_ROGUE_COMBAT                = 76806,
    MASTERY_ROGUE_SUBTLETY              = 76808,

    MASTERY_PRIEST_DISCIPLINE           = 77484,
    MASTERY_PRIEST_HOLY                 = 77485,
    MASTERY_PRIEST_SHADOW               = 77486,

    MASTERY_DEATHKNIGHT_BLOOD           = 77513,
    MASTERY_DEATHKNIGHT_FROST           = 77514,
    MASTERY_DEATHKNIGHT_UNHOLY          = 77515,

    MASTERY_SHAMAN_ELEMENTAL            = 77222,
    MASTERY_SHAMAN_ENHANCEMENT          = 77223,
    MASTERY_SHAMAN_RESTORATION          = 77226,

    MASTERY_MAGE_ARCANE                 = 76547,
    MASTERY_MAGE_FIRE                   = 12846,
    MASTERY_MAGE_FROST                  = 76613,

    MASTERY_WARLOCK_AFFLICTION          = 77215,
    MASTERY_WARLOCK_DEMONOLOGY          = 77219,
    MASTERY_WARLOCK_DESTRUCTION         = 77220,

    MASTERY_MONK_BREWMASTER             = 117906,
    MASTERY_MONK_MISTWEAVER             = 117907,
    MASTERY_MONK_WINDWALKER             = 115636,

    MASTERY_DRUID_BALANCE               = 77492,
    MASTERY_DRUID_FERAL                 = 77493,
    MASTERY_DRUID_GUARDIAN              = 77494,
    MASTERY_DRUID_RESTORATION           = 77495
};

enum WarriorSpells
{
	SPELL_WARR_ENRAGE		= 12880,
	SPELL_WARR_RAGING_BLOW	= 131116,
};

enum PaladinSpells
{
	SPELL_PAL_HAND_OF_LIGHT_TRIGGERED	= 96172,
	SPELL_PAL_CRUSADER_STRIKE			= 35395,
	SPELL_PAL_HAMMER_OF_THE_RIGHTEOUS	= 53595,
	SPELL_PAL_HAMMER_OF_WRATH			= 24275,
	SPELL_PAL_TEMPLARS_VERDICT			= 85256,
	SPELL_PAL_DIVINE_STORM				= 53385,
};

// Warrior spell : Enrage 12880
class spell_mastery_unshackled_fury : public SpellScriptLoader
{
    public:
        spell_mastery_unshackled_fury() : SpellScriptLoader("spell_mastery_unshackled_fury") { }

        class spell_mastery_unshackled_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mastery_unshackled_fury_AuraScript);

			void EffectApply (AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
			{
				Player* player = GetCaster()->ToPlayer();

				if (player->GetPrimaryTalentTree(player->GetActiveSpec()) == TALENT_TREE_WARRIOR_FURY)
				{
					player->CastSpell(player, SPELL_WARR_RAGING_BLOW, true);
					player->SetAuraStack(SPELL_WARR_RAGING_BLOW, player, 2);
				}
			}

            void CalculateAmount(AuraEffect const* aurEff, int32 & amount, bool & /*canBeRecalculated*/)
            {
                if (Player* player = GetCaster()->ToPlayer())
                    if (player->HasAura(MASTERY_WARRIOR_FURY) && player->getLevel() >= 80)
						amount = int32(player->GetFloatValue(PLAYER_MASTERY) * 1.4f / 100);
            }

            void Register()
            {
				OnEffectApply += AuraEffectApplyFn(spell_mastery_unshackled_fury_AuraScript::EffectApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mastery_unshackled_fury_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mastery_unshackled_fury_AuraScript();
        }
};

// Paladin spell : Hand of Light
class spell_mastery_hand_of_light : public SpellScriptLoader
{
    public:
        spell_mastery_hand_of_light() : SpellScriptLoader("spell_mastery_hand_of_light") { }

        class spell_mastery_hand_of_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mastery_hand_of_light_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
				if (!sSpellMgr->GetSpellInfo(SPELL_PAL_CRUSADER_STRIKE))
                    return false;
				if (!sSpellMgr->GetSpellInfo(SPELL_PAL_HAMMER_OF_THE_RIGHTEOUS))
                    return false;
				if (!sSpellMgr->GetSpellInfo(SPELL_PAL_HAMMER_OF_WRATH))
                    return false;
				if (!sSpellMgr->GetSpellInfo(SPELL_PAL_DIVINE_STORM))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
				int32 bp0 = 0;
				
				if (Player* player = GetCaster()->ToPlayer())
				{
					float mastery = player->GetFloatValue(PLAYER_MASTERY) * 2.1f / 100;

					if  (sSpellMgr->GetSpellInfo(SPELL_PAL_HAMMER_OF_WRATH))
					{
						int32 damage = int32(1.6f * player->GetTotalAttackPowerValue(BASE_ATTACK));
						bp0 = int32(mastery * damage);
						player->CastCustomSpell(player->getVictim(), SPELL_PAL_HAND_OF_LIGHT_TRIGGERED, &bp0, NULL, NULL, true);
					}
					else
					{
						bp0 = int32(mastery * GetHitDamage());
						player->CastCustomSpell(player->getVictim(), SPELL_PAL_HAND_OF_LIGHT_TRIGGERED, &bp0, NULL, NULL, true);
					}
				}
			}

            void Register()
            {
                OnHit += SpellHitFn(spell_mastery_hand_of_light_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mastery_hand_of_light_SpellScript();
        }
};

void AddSC_masteries_spell_scripts()
{
	new spell_mastery_unshackled_fury();
	new spell_mastery_hand_of_light();
}