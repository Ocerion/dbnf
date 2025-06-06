/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
 * Tricops and Fireblade                                      |             *
 * ------------------------------------------------------------------------ *
 * Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
 * Chastain, Michael Quan, and Mitchell Tse.                                *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
 * Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
 * ------------------------------------------------------------------------ *
 * 			Table load/save Module				    *
 ****************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"
#ifdef USE_IMC
#include "imc-mercbase.h"
#include "icec-mercbase.h"
#include "changes.c"
#endif

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

bool load_race_file( char *fname );
void write_race_file( int ra );


/* global variables */
int top_sn;
int top_herb;
int MAX_PC_CLASS;
int MAX_PC_RACE;

SKILLTYPE *skill_table[MAX_SKILL];
struct class_type *class_table[MAX_CLASS];
RACE_TYPE *race_table[MAX_RACE];
char *title_table[MAX_CLASS][MAX_LEVEL + 1][2];
SKILLTYPE *herb_table[MAX_HERB];
SKILLTYPE *disease_table[MAX_DISEASE];

LANG_DATA *first_lang;
LANG_DATA *last_lang;

char *const skill_tname[] = { "unknown", "Spell", "Skill", "Ability", "Weapon", "Tongue", "Herb", "Racial", "Disease" };

SPELL_FUN *spell_function( char *name )
{
  if( !str_cmp( name, "spell_smaug" ) )
    return spell_smaug;

  if( !str_cmp( name, "reserved" ) )
    return NULL;
  if( !str_cmp( name, "spell_null" ) )
    return spell_null;
  if( !str_cmp( name, "spell_sensu_bean" ) )
    return spell_sensu_bean;
  return spell_notfound;
}

DO_FUN *skill_function( char *name )
{
  switch ( name[3] )
  {
/*//T1*/
    case 'a':
      if( !str_cmp( name, "do_angelicrapture" ) )
        return do_angelicrapture;
      if( !str_cmp( name, "do_aiminglaser" ) )
        return do_aiminglaser;
      if( !str_cmp( name, "do_aassign" ) )
        return do_aassign;
      if( !str_cmp( name, "do_absorb" ) )
        return do_absorb;
      if( !str_cmp( name, "do_autotrain" ) )
        return do_autotrain;
      if( !str_cmp( name, "do_accelerate" ) )
        return do_accelerate;
      if( !str_cmp( name, "do_accessories" ) )
        return do_accessories;
      if(!str_cmp(name, "do_abandonrank"))
	return do_abandonrank;
      if( !str_cmp( name, "do_acid_burn" ) )
        return do_acid_burn;
      if( !str_cmp( name, "do_acid_slash" ) )
        return do_acid_slash;
      if( !str_cmp( name, "do_add_imm_host" ) )
        return do_add_imm_host;
      if( !str_cmp( name, "do_addchange" ) )
        return do_addchange;
      if( !str_cmp( name, "do_addimmchange" ) )
        return do_addimmchange;
      if( !str_cmp( name, "do_addpilot" ) )
        return do_addpilot;
      if( !str_cmp( name, "do_admintalk" ) )
        return do_admintalk;
      if( !str_cmp( name, "do_addlessj" ) )
        return do_addlessj;
      if( !str_cmp( name, "do_adminto" ) )
        return do_adminto;
      if( !str_cmp( name, "do_advance" ) )
        return do_advance;
      if( !str_cmp( name, "do_affected" ) )
        return do_affected;
      if( !str_cmp( name, "do_afk" ) )
        return do_afk;
      if( !str_cmp( name, "do_ahelp" ) )
        return do_ahelp;
      if( !str_cmp( name, "do_aid" ) )
        return do_aid;
      if( !str_cmp( name, "do_alist" ) )
        return do_alist;
      if( !str_cmp( name, "do_aligncheck" ) )
        return do_aligncheck;
      if( !str_cmp( name, "do_allow" ) )
        return do_allow;
      if( !str_cmp( name, "do_allhigh" ) )
        return do_allhigh;
      if( !str_cmp( name, "do_allships" ) )
        return do_allships;
      if( !str_cmp( name, "do_allspeeders" ) )
        return do_allspeeders;
      if( !str_cmp( name, "do_androidfuse" ) )
        return do_androidfuse;
      if( !str_cmp( name, "do_analyze" ) )
        return do_analyze;
      if( !str_cmp( name, "do_ansi" ) )
        return do_ansi;
      if( !str_cmp( name, "do_answer" ) )
        return do_answer;
      if( !str_cmp( name, "do_apply" ) )
        return do_apply;
      if( !str_cmp( name, "do_appraise" ) )
        return do_appraise;
      if( !str_cmp( name, "do_areas" ) )
        return do_areas;
      if( !str_cmp( name, "do_aset" ) )
        return do_aset;
      if( !str_cmp( name, "do_ask" ) )
        return do_ask;
      if( !str_cmp( name, "do_astat" ) )
        return do_astat;
      if( !str_cmp( name, "do_at" ) )
        return do_at;
      if( !str_cmp( name, "do_atb" ) )
        return do_atb;
      if( !str_cmp( name, "do_atobj" ) )
        return do_atobj;
      if( !str_cmp( name, "do_auction" ) )
        return do_auction;
      if( !str_cmp( name, "do_aura_set" ) )
        return do_aura_set;
      if( !str_cmp( name, "do_authorize" ) )
        return do_authorize;
      if( !str_cmp( name, "do_autopilot" ) )
        return do_autopilot;
      if( !str_cmp( name, "do_autorecharge" ) )
        return do_autorecharge;
      if( !str_cmp( name, "do_autotrack" ) )
        return do_autotrack;
      if( !str_cmp( name, "do_autotrain" ) )
        return do_autotrain;
      if( !str_cmp( name, "do_avtalk" ) )
        return do_avtalk;
      if( !str_cmp( name, "do_ancientevolution" ) )
        return do_ancientevolution;
      break;
    case 'b':
      if( !str_cmp( name, "do_backup" ) )
        return do_backup;
      if( !str_cmp( name, "do_battery" ) )
        return do_battery;
      if( !str_cmp( name, "do_blast_zone" ) )
        return do_blast_zone;
      if( !str_cmp( name, "do_banish" ) )
        return do_banish;
      if( !str_cmp( name, "do_big_bang" ) )
        return do_big_bang;
      if( !str_cmp( name, "do_burning_attack" ) )
        return do_burning_attack;
      if( !str_cmp( name, "do_buuball" ) )
        return do_buuball;
      if( !str_cmp( name, "do_buffet" ) )
        return do_buffet;
      if( !str_cmp( name, "do_backstab" ) )
        return do_backstab;
      if( !str_cmp( name, "do_balzhur" ) )
        return do_balzhur;
      if( !str_cmp( name, "do_bamfin" ) )
        return do_bamfin;
      if( !str_cmp( name, "do_bamfout" ) )
        return do_bamfout;
      if( !str_cmp( name, "do_ban" ) )
        return do_ban;
      if( !str_cmp( name, "do_bash" ) )
        return do_bash;
      if( !str_cmp( name, "do_bashdoor" ) )
        return do_bashdoor;
      if( !str_cmp( name, "do_bbk" ) )
        return do_bbk;
      if( !str_cmp( name, "do_biodrain" ) )
        return do_biodrain;
      if( !str_cmp( name, "do_bpoint" ) )
        return do_bpoint;
      if( !str_cmp( name, "do_beep" ) )
        return do_beep;
      if( !str_cmp( name, "do_berserk" ) )
        return do_berserk;
      if( !str_cmp( name, "do_bestow" ) )
        return do_bestow;
      if( !str_cmp( name, "do_bestowarea" ) )
        return do_bestowarea;
      if( !str_cmp( name, "do_bio" ) )
        return do_bio;
      if( !str_cmp( name, "do_bite" ) )
        return do_bite;
      if( !str_cmp( name, "do_bloodlet" ) )
        return do_bloodlet;
      if( !str_cmp( name, "do_bless" ) )
        return do_bless;
      if( !str_cmp( name, "do_board" ) )
        return do_board;
      if( !str_cmp( name, "do_boards" ) )
        return do_boards;
      if( !str_cmp( name, "do_bombarrow" ) )
        return do_bombarrow;
      if( !str_cmp( name, "do_bodybag" ) )
        return do_bodybag;
      if( !str_cmp( name, "do_bolt" ) )
        return do_bolt;
      if( !str_cmp( name, "do_botwatch" ) )
        return do_botwatch;
      if( !str_cmp( name, "do_bounty" ) )
        return do_bounty;
      if( !str_cmp( name, "do_brandish" ) )
        return do_brandish;
      if( !str_cmp( name, "do_brew" ) )
        return do_brew;
      if( !str_cmp( name, "do_bset" ) )
        return do_bset;
      if( !str_cmp( name, "do_bstat" ) )
        return do_bstat;
      if( !str_cmp( name, "do_bug" ) )
        return do_bug;
      if( !str_cmp( name, "do_bury" ) )
        return do_bury;
      if( !str_cmp( name, "do_buy" ) )
        return do_buy;
//      if( !str_cmp( name, "do_bank" ) )
//        return do_bank;
      if( !str_cmp( name, "do_buyship" ) )
        return do_buyship;
      break;
    case 'c':
      if( !str_cmp( name, "do_changes" ) )
        return do_changes;
      if( !str_cmp( name, "do_chedit" ) )
        return do_chedit;
      if( !str_cmp( name, "do_claimrank" ) )
        return do_claimrank;
      if( !str_cmp( name, "do_clawstrike" ) )
        return do_clawstrike;
      if( !str_cmp( name, "do_cruise_punch" ) )
        return do_cruise_punch;
      if( !str_cmp( name, "do_callship" ) )
        return do_callship;
      if( !str_cmp( name, "do_calculate" ) )
        return do_calculate;
      if( !str_cmp( name, "do_cache" ) )
        return do_cache;
      if( !str_cmp( name, "do_candy_blast" ) )
        return do_candy_blast;
      if( !str_cmp( name, "do_cast" ) )
        return do_cast;
      if( !str_cmp( name, "do_cedit" ) )
        return do_cedit;
      if( !str_cmp( name, "do_censor" ) )
        return do_censor;
      if( !str_cmp( name, "do_chaff" ) )
        return do_chaff;
      if( !str_cmp( name, "do_charge" ) )
        return do_charge;
      if( !str_cmp( name, "do_chaos" ) )
        return do_chaos;
      if( !str_cmp( name, "do_channels" ) )
        return do_channels;
      if( !str_cmp( name, "do_chat" ) )
        return do_chat;
      if( !str_cmp( name, "do_check_vnums" ) )
        return do_check_vnums;
      if( !str_cmp( name, "do_check_ld" ) )
        return do_check_ld;
      if( !str_cmp( name, "do_chou_kamehameha" ) )
        return do_chou_kamehameha;
      if( !str_cmp( name, "do_circle" ) )
        return do_circle;
      if( !str_cmp( name, "do_clanadmin" ) )
        return do_clanadmin;
      if( !str_cmp( name, "do_clanrecall" ) )
        return do_clanrecall;
      if( !str_cmp( name, "do_clandeposit" ) )
        return do_clandeposit;
      if( !str_cmp( name, "do_clanwithdraw" ) )
        return do_clanwithdraw;
      if( !str_cmp( name, "do_claninfo" ) )
        return do_claninfo;
      if( !str_cmp( name, "do_clans" ) )
        return do_clans;
      if( !str_cmp( name, "do_clantalk" ) )
        return do_clantalk;
      if( !str_cmp( name, "do_cleanse" ) )
        return do_cleanse;
      if( !str_cmp( name, "do_cleaver" ) )
        return do_cleaver;
      if( !str_cmp( name, "do_clear" ) )
        return do_clear;
      if( !str_cmp( name, "do_clearhiscore" ) )
        return do_clearhiscore;
      if( !str_cmp( name, "do_climate" ) )
        return do_climate;
      if( !str_cmp( name, "do_climb" ) )
        return do_climb;
      if( !str_cmp( name, "do_clone" ) )
        return do_clone;
      if( !str_cmp( name, "do_clones" ) ) /* Command for checking for duplicate IPs --Arbin */
        return do_clones;
      if( !str_cmp( name, "do_close" ) )
        return do_close;
      if( !str_cmp( name, "do_closehatch" ) )
        return do_closehatch;
      if( !str_cmp( name, "do_cmdtable" ) )
        return do_cmdtable;
      if( !str_cmp( name, "do_cmenu" ) )
        return do_cmenu;
      if( !str_cmp( name, "do_colorize" ) )
        return do_colorize;
      if( !str_cmp( name, "do_corpse" ) )
        return do_corpse;
      if( !str_cmp( name, "do_combat" ) )
        return do_combat;
      if( !str_cmp( name, "do_commands" ) )
        return do_commands;
      if( !str_cmp( name, "do_comment" ) )
        return do_comment;
      if( !str_cmp( name, "do_compare" ) )
        return do_compare;
#ifdef MCCP
      if( !str_cmp( name, "do_compress" ) )
        return do_compress;
#endif
      if( !str_cmp( name, "do_config" ) )
        return do_config;
      if( !str_cmp( name, "do_connect" ) )
        return do_connect;
      if( !str_cmp( name, "do_consider" ) )
        return do_consider;
      if( !str_cmp( name, "do_cook" ) )
        return do_cook;
      if( !str_cmp( name, "do_copyship" ) )
        return do_copyship;
     if( !str_cmp( name, "do_copy_ability" ) )
        return do_copy_ability;
      if( !str_cmp( name, "do_council_induct" ) )
        return do_council_induct;
      if( !str_cmp( name, "do_council_outcast" ) )
        return do_council_outcast;
      if( !str_cmp( name, "do_councils" ) )
        return do_councils;
      if( !str_cmp( name, "do_counciltalk" ) )
        return do_counciltalk;
      if( !str_cmp( name, "do_credits" ) )
        return do_credits;
      if( !str_cmp( name, "do_cset" ) )
        return do_cset;
      break;
    case 'd':
      if( !str_cmp( name, "do_dragon_thunder" ) )
        return do_dragon_thunder;
      if( !str_cmp( name, "do_dragonfire" ) )
        return do_dragonfire;
      if( !str_cmp( name, "do_deathbeam" ) )
        return do_deathbeam;
      if( !str_cmp( name, "do_ddmb" ) )
        return do_ddmb;
      if( !str_cmp( name, "do_daichiretsuzan" ) )
        return do_daichiretsuzan;
      if( !str_cmp( name, "do_devil_flash" ) )
        return do_devil_flash;
      if( !str_cmp( name, "do_defuse" ) )
        return do_defuse;
      if( !str_cmp( name, "do_dragonballscatter" ) )
        return do_dragonballscatter;
      if( !str_cmp( name, "do_dbwhere" ) )
        return do_dbwhere;
      if( !str_cmp( name, "do_doublekick" ) )
        return do_doublekick;
      if( !str_cmp( name, "do_doublepunch" ) )
        return do_doublepunch;
      if( !str_cmp( name, "do_dbradar" ) )
        return do_dbradar;
      if( !str_cmp( name, "do_drp" ) )
        return do_drp;
      if( !str_cmp( name, "do_darkness_flare" ) )
        return do_darkness_flare;
      if( !str_cmp( name, "do_destructive_wave" ) )
        return do_destructive_wave;
      if( !str_cmp( name, "do_dodon_ray" ) )
        return do_dodon_ray;
      if( !str_cmp( name, "do_desperation" ) )
        return do_desperation;
      if( !str_cmp( name, "do_dd" ) )
        return do_dd;
      if( !str_cmp( name, "do_ddd" ) )
        return do_ddd;
      if( !str_cmp( name, "do_death_ball" ) )
        return do_death_ball;
      if( !str_cmp( name, "do_deities" ) )
        return do_deities;
      if( !str_cmp( name, "do_dekumask" ) )
        return do_dekumask;
      if( !str_cmp( name, "do_delay" ) )
        return do_delay;
      if( !str_cmp( name, "do_delete" ) )
        return do_delete;
      if( !str_cmp( name, "do_demonic_eclipse" ) )
        return do_demonic_eclipse;
      if( !str_cmp( name, "do_demonic_embrace" ) )
        return do_demonic_embrace;
      if( !str_cmp( name, "do_demonweapon" ) )
        return do_demonweapon;
      if( !str_cmp( name, "do_demon_wave" ) )
        return do_demon_wave;
      if( !str_cmp( name, "do_deny" ) )
        return do_deny;
      if( !str_cmp( name, "do_description" ) )
        return do_description;
      if( !str_cmp( name, "do_description1" ) )
        return do_description1;
      if( !str_cmp( name, "do_description2" ) )
        return do_description2;
      if( !str_cmp( name, "do_description3" ) )
        return do_description3;
      if( !str_cmp( name, "do_description4" ) )
        return do_description4;
      if( !str_cmp( name, "do_description5" ) )
        return do_description5;
      if( !str_cmp( name, "do_destro" ) )
        return do_destro;
      if( !str_cmp( name, "do_destroy" ) )
        return do_destroy;
      if( !str_cmp( name, "do_detrap" ) )
        return do_detrap;
      if( !str_cmp( name, "do_devote" ) )
        return do_devote;
      if( !str_cmp( name, "do_diagnose" ) )
        return do_diagnose;
      if( !str_cmp( name, "do_dig" ) )
        return do_dig;
      if( !str_cmp( name, "do_disarm" ) )
        return do_disarm;
      if( !str_cmp( name, "do_disconnect" ) )
        return do_disconnect;
      if( !str_cmp( name, "do_dismiss" ) )
        return do_dismiss;
      if( !str_cmp( name, "do_dismount" ) )
        return do_dismount;
      if( !str_cmp( name, "do_divinewrath" ) )
        return do_divinewrath;
      if( !str_cmp( name, "do_divorce" ) )
        return do_divorce;
      if( !str_cmp( name, "do_dmesg" ) )
        return do_dmesg;
      if( !str_cmp( name, "do_dnd" ) )
        return do_dnd;
      if( !str_cmp( name, "do_dnsusers" ) )
        return do_dnsusers;
      if( !str_cmp( name, "do_down" ) )
        return do_down;
      if( !str_cmp( name, "do_drag" ) )
        return do_drag;
      if( !str_cmp( name, "do_drink" ) )
        return do_drink;
      if( !str_cmp( name, "do_drive" ) )
        return do_drive;
      if( !str_cmp( name, "do_drop" ) )
        return do_drop;
      if( !str_cmp( name, "do_duplicate" ) )
        return do_duplicate;
      break;
    case 'e':
      if( !str_cmp( name, "do_earth_line_blast" ) )
        return do_earth_line_blast;
      if( !str_cmp( name, "do_enhance" ) )
        return do_enhance;
      if( !str_cmp( name, "do_eliminate" ) )
        return do_eliminate;
      if( !str_cmp( name, "do_east" ) )
        return do_east;
      if( !str_cmp( name, "do_eat" ) )
        return do_eat;
      if( !str_cmp( name, "do_ech" ) )
        return do_ech;
      if( !str_cmp( name, "do_echo" ) )
        return do_echo;
      if( !str_cmp( name, "do_ehb" ) )
        return do_ehb;
      if( !str_cmp( name, "do_electric_shield" ) )
        return do_electric_shield;
      if( !str_cmp( name, "do_elevate" ) )
        return do_elevate;
      if( !str_cmp( name, "do_email" ) )
        return do_email;
      if( !str_cmp( name, "do_emote" ) )
        return do_emote;
      if( !str_cmp( name, "do_empty" ) )
        return do_empty;
      if( !str_cmp( name, "do_energy_ball" ) )
        return do_energy_ball;
      if( !str_cmp( name, "do_energy_ring" ) )
        return do_energy_ring;
      if( !str_cmp( name, "do_enter" ) )
        return do_enter;
      if( !str_cmp( name, "do_equipment" ) )
        return do_equipment;
      if( !str_cmp( name, "do_eruption" ) )
        return do_eruption;
      if( !str_cmp( name, "do_evilboost" ) )
        return do_evilboost;
      if( !str_cmp( name, "do_evilsurge" ) )
        return do_evilsurge;
      if( !str_cmp( name, "do_eviloverload" ) )
        return do_eviloverload;
      if( !str_cmp( name, "do_examine" ) )
        return do_examine;
      if( !str_cmp( name, "do_exits" ) )
        return do_exits;
      if( !str_cmp( name, "do_eye_beam" ) )
        return do_eye_beam;
      if( !str_cmp( name, "do_evolve" ) )
        return do_evolve;
      if( !str_cmp( name, "do_extreme" ) )
        return do_extreme;
      if( !str_cmp( name, "do_essj" ) )
        return do_essj;
      break;
    case 'f':
      if( !str_cmp( name, "do_finisher" ) )
        return do_finisher;
      if( !str_cmp( name, "do_fury" ) )
        return do_fury;
      if( !str_cmp( name, "do_flare" ) )
        return do_flare;
      if( !str_cmp( name, "do_fakemoon" ) )
        return do_fakemoon;
      if( !str_cmp( name, "do_faroreswind" ) )
        return do_faroreswind;
      if( !str_cmp( name, "do_fusiondance" ) )
        return do_fusiondance;
      if( !str_cmp( name, "do_finalshine" ) )
        return do_finalshine;
      if( !str_cmp( name, "do_finishing_buster" ) )
        return do_finishing_buster;
      if( !str_cmp( name, "do_feed" ) )
        return do_feed;
      if( !str_cmp( name, "do_ff" ) )
        return do_ff;
      if( !str_cmp( name, "do_fiercedeity" ) )
        return do_fiercedeity;
      if( !str_cmp( name, "do_fill" ) )
        return do_fill;
      if( !str_cmp( name, "do_findnote" ) )
        return do_findnote;
      if( !str_cmp( name, "do_finger" ) )
        return do_finger;
      if( !str_cmp( name, "do_finger_beam" ) )
        return do_finger_beam;
      if( !str_cmp( name, "do_fire" ) )
        return do_fire;
      if( !str_cmp( name, "do_firearrow" ) )
        return do_firearrow;
      if( !str_cmp( name, "do_fireball" ) )
        return do_fireball;
      if( !str_cmp( name, "do_fireforge" ) )
        return do_fireforge;
      if( !str_cmp( name, "do_firestorm" ) )
        return do_firestorm;
      if( !str_cmp( name, "do_fixchar" ) )
        return do_fixchar;
      if( !str_cmp( name, "do_fixed" ) )
        return do_fixed;
      if( !str_cmp( name, "do_flee" ) )
        return do_flee;
      if( !str_cmp( name, "do_fly" ) )
        return do_fly;
      if( !str_cmp( name, "do_foldarea" ) )
        return do_foldarea;
      if( !str_cmp( name, "do_follow" ) )
        return do_follow;
      if( !str_cmp( name, "do_for" ) )
        return do_for;
     if( !str_cmp( name, "do_forget" ) )
        return do_forget;
      if( !str_cmp( name, "do_force" ) )
        return do_force;
      if( !str_cmp( name, "do_forceclose" ) )
        return do_forceclose;
      if( !str_cmp( name, "do_form_password" ) )
        return do_form_password;
      if( !str_cmp( name, "do_fos" ) )
        return do_fos;
      if( !str_cmp( name, "do_fprompt" ) )
        return do_fprompt;
      if( !str_cmp( name, "do_fquit" ) )
        return do_fquit;
      if( !str_cmp( name, "do_freeze" ) )
        return do_freeze;
      if( !str_cmp( name, "do_fshow" ) )
        return do_fshow;
      if( !str_cmp( name, "do_fslay" ) )
        return do_fslay;
      if( !str_cmp( name, "do_fullsave" ) )
        return do_fullsave;
      if( !str_cmp( name, "do_ftower" ) )
        return do_ftower;
      break;
    case 'g':
      if( !str_cmp( name, "do_gocials" ) )
        return do_gocials;
      if( !str_cmp( name, "do_gust" ) )
        return do_gust;
      if( !str_cmp( name, "do_ghunter" ) )
        return do_ghunter;
      if( !str_cmp( name, "do_giga1" ) )
        return do_giga1;
      if( !str_cmp( name, "do_giga2" ) )
        return do_giga2;
      if( !str_cmp( name, "do_games" ) )
        return do_games;
      if( !str_cmp( name, "do_gained" ) )
        return do_gained;
      if( !str_cmp( name, "do_gallic_gun" ) )
        return do_gallic_gun;
      if( !str_cmp( name, "do_gaso" ) )
        return do_gaso;
      if( !str_cmp( name, "do_gasattack" ) )
        return do_gasattack;
      if( !str_cmp( name, "do_get" ) )
        return do_get;
      if( !str_cmp( name, "do_genocide_attack" ) )
        return do_genocide_attack;
      if( !str_cmp( name, "do_gfighting" ) )
        return do_gfighting;
      if( !str_cmp( name, "do_ghost" ) )
        return do_ghost;
      if( !str_cmp( name, "do_give" ) )
        return do_give;
      if( !str_cmp( name, "do_giant_size" ) )
        return do_giant_size;
      if( !str_cmp( name, "do_glance" ) )
        return do_glance;
      if( !str_cmp( name, "do_global_boards" ) )
        return do_global_boards;
      if( !str_cmp( name, "do_global_note" ) )
        return do_global_note;
      if( !str_cmp( name, "do_godset" ) )
        return do_godset;
      if( !str_cmp( name, "do_godstat" ) )
        return do_godstat;
      if( !str_cmp( name, "do_godwho" ) )
        return do_godwho;
      if( !str_cmp( name, "do_gohome" ) )
        return do_gohome;
      if( !str_cmp( name, "do_gold" ) )
        return do_gold;
      if( !str_cmp( name, "do_goronmask" ) )
        return do_goronmask;
      if( !str_cmp( name, "do_goto" ) )
        return do_goto;
      if( !str_cmp( name, "do_gouge" ) )
        return do_gouge;
      if( !str_cmp( name, "do_grant" ) )
        return do_grant;
      if( !str_cmp( name, "do_growth" ) )
        return do_growth;
      if( !str_cmp( name, "do_group" ) )
        return do_group;
      if( !str_cmp( name, "do_grub" ) )
        return do_grub;
      if( !str_cmp( name, "do_gtell" ) )
        return do_gtell;
      if( !str_cmp( name, "do_guilds" ) )
        return do_guilds;
      if( !str_cmp( name, "do_guildtalk" ) )
        return do_guildtalk;
      if( !str_cmp( name, "do_gwhere" ) )
        return do_gwhere;
      break;
    case 'h':
      if( !str_cmp( name, "do_hbomb" ) )
        return do_hbomb;
      if( !str_cmp( name, "do_htorch" ) )
        return do_htorch;
      if( !str_cmp( name, "do_headbutt" ) )
        return do_headbutt;
      if( !str_cmp( name, "do_heaven_splitter_cannon" ) )
        return do_heaven_splitter_cannon;
      if( !str_cmp( name, "do_hellzone_grenade" ) )
        return do_hellzone_grenade;
      if( !str_cmp( name, "do_hcchat" ) )
        return do_hcchat;
      if( !str_cmp( name, "do_hedit" ) )
        return do_hedit;
      if( !str_cmp( name, "do_hyper" ) )
        return do_hyper;
      if( !str_cmp( name, "do_hell" ) )
        return do_hell;
      if( !str_cmp( name, "do_hells_flash" ) )
        return do_hells_flash;
      if( !str_cmp( name, "do_help" ) )
        return do_help;
      if( !str_cmp( name, "do_hide" ) )
        return do_hide;
      if( !str_cmp( name, "do_hiscore" ) )
        return do_hiscore;
      if( !str_cmp( name, "do_hiscoset" ) )
        return do_hiscoset;
      if( !str_cmp( name, "do_hitall" ) )
        return do_hitall;
      if( !str_cmp( name, "do_hl" ) )
        return do_hl;
      if( !str_cmp( name, "do_hlist" ) )
        return do_hlist;
      if( !str_cmp( name, "do_holylight" ) )
        return do_holylight;
      if( !str_cmp( name, "do_homebuy" ) )
        return do_homebuy;
      if( !str_cmp( name, "do_homepage" ) )
        return do_homepage;
      if( !str_cmp( name, "do_honoo" ) )
        return do_honoo;
      if( !str_cmp( name, "do_hookshot" ) )
        return do_hookshot;		
      if( !str_cmp( name, "do_hotboot" ) )
        return do_hotboot;
      if( !str_cmp( name, "do_house" ) )
        return do_house;
      if( !str_cmp( name, "do_hoverboots" ) )
        return do_hoverboots;
      if( !str_cmp( name, "do_hset" ) )
        return do_hset;
      if( !str_cmp( name, "do_hyperspace" ) )
        return do_hyperspace;
      if( !str_cmp( name, "do_hssj" ) )
        return do_hssj;
      break;
    case 'i':
      if( !str_cmp( name, "do_innerwill" ) )
        return do_innerwill;
      if( !str_cmp( name, "do_icearrow" ) )
        return do_icearrow;
      if( !str_cmp( name, "do_icer_transform_2" ) )
        return do_icer_transform_2;
      if( !str_cmp( name, "do_icer_transform_3" ) )
        return do_icer_transform_3;
      if( !str_cmp( name, "do_instant_trans" ) )
        return do_instant_trans;
      if( !str_cmp( name, "do_icer_transform_4" ) )
        return do_icer_transform_4;
      if( !str_cmp( name, "do_icer_transform_5" ) )
        return do_icer_transform_5;
      // if ( !str_cmp( name, "do_ichannels"                 ))    return do_ichannels;
      // if ( !str_cmp( name, "do_icommand"                  ))    return do_icommand;
      if( !str_cmp( name, "do_icq_number" ) )
        return do_icq_number;
      if( !str_cmp( name, "do_ide" ) )
        return do_ide;
      if( !str_cmp( name, "do_idea" ) )
        return do_idea;
      if( !str_cmp( name, "do_ignore" ) )
        return do_ignore;
      // if ( !str_cmp( name, "do_ilist"                     ))    return do_ilist;
      // if ( !str_cmp( name, "do_imc"                       ))    return do_imc;
      // if ( !str_cmp( name, "do_imclist"                   ))    return do_imclist;
      if( !str_cmp( name, "do_imm_morph" ) )
        return do_imm_morph;
      if( !str_cmp( name, "do_imm_unmorph" ) )
        return do_imm_unmorph;
      if( !str_cmp( name, "do_immtalk" ) )
        return do_immtalk;
      if( !str_cmp( name, "do_immortalaurareset" ) )
        return do_immortalaurareset;
      if( !str_cmp( name, "do_incognito" ) )
        return do_incognito;
      if( !str_cmp( name, "do_induct" ) )
        return do_induct;
      if( !str_cmp( name, "do_info" ) )
        return do_info;
      if( !str_cmp( name, "do_integrate" ) )
        return do_integrate;
      if( !str_cmp( name, "do_installarea" ) )
        return do_installarea;
      if( !str_cmp( name, "do_instaroom" ) )
        return do_instaroom;
      if( !str_cmp( name, "do_instazone" ) )
        return do_instazone;
      if( !str_cmp( name, "do_inventory" ) )
        return do_inventory;
      if( !str_cmp( name, "do_invis" ) )
        return do_invis;
      if( !str_cmp( name, "do_ipcompare" ) )
        return do_ipcompare;
      // if ( !str_cmp( name, "do_isetup"                    ))    return do_isetup;
      // if ( !str_cmp( name, "do_istats"                    ))    return do_istats;
      break;
    case 'j':
      if( !str_cmp( name, "do_joinclan" ) )
        return do_joinclan;
      if( !str_cmp( name, "do_jday" ) )
        return do_jday;
      if( !str_cmp( name, "do_jibaku" ) )
        return do_jibaku;
      break;
    case 'k':
      if( !str_cmp( name, "do_kaioken" ) )
        return do_kaioken;
      if( !str_cmp( name, "do_kaiocreate" ) )
        return do_kaiocreate;
      if( !str_cmp( name, "do_kairestore" ) )
        return do_kairestore;
      if( !str_cmp( name, "do_kakusandan" ) )
        return do_kakusandan;

      if( !str_cmp( name, "do_kamehameha" ) )
        return do_kamehameha;
      if( !str_cmp( name, "do_khistory" ) )
        return do_khistory;
      if( !str_cmp( name, "do_kick" ) )
        return do_kick;
      if( !str_cmp( name, "do_kill" ) )
        return do_kill;
      if( !str_cmp( name, "do_ki_burst" ) )
        return do_ki_burst;
      if( !str_cmp( name, "do_ki_heal" ) )
        return do_ki_heal;
      break;
    case 'l':
      if( !str_cmp( name, "do_lshield" ) )
        return do_lshield;
      if( !str_cmp( name, "do_land" ) )
        return do_land;
      if( !str_cmp( name, "do_languages" ) )
        return do_languages;
      if( !str_cmp( name, "do_last" ) )
        return do_last;
      if( !str_cmp( name, "do_launch" ) )
        return do_launch;
      if( !str_cmp( name, "do_laws" ) )
        return do_laws;
      if( !str_cmp( name, "do_leave" ) )
        return do_leave;
      if( !str_cmp( name, "do_leaveship" ) )
        return do_leaveship;
      if( !str_cmp( name, "do_level" ) )
        return do_level;
      if( !str_cmp( name, "do_light" ) )
        return do_light;
      if( !str_cmp( name, "do_lightarrow" ) )
        return do_lightarrow;
      if( !str_cmp( name, "do_list" ) )
        return do_list;
      if( !str_cmp( name, "do_litterbug" ) )
        return do_litterbug;
      if( !str_cmp( name, "do_loadarea" ) )
        return do_loadarea;
      if( !str_cmp( name, "do_loadup" ) )
        return do_loadup;
      if( !str_cmp( name, "do_lock" ) )
        return do_lock;
      if( !str_cmp( name, "do_log" ) )
        return do_log;
      if( !str_cmp( name, "do_look" ) )
        return do_look;
      if( !str_cmp( name, "do_lookmap" ) )
        return do_lookmap;
      if( !str_cmp( name, "do_loop" ) )
        return do_loop;
      if( !str_cmp( name, "do_low_purge" ) )
        return do_low_purge;
      if( !str_cmp( name, "do_lssj" ) )
        return do_lssj;
      break;
    case 'm':
      if( !str_cmp( name, "do_mechaevolution" ) )
        return do_mechaevolution;
      if( !str_cmp( name, "do_magmatouch" ) )
        return do_magmatouch;
      // if ( !str_cmp( name, "do_mailqueue"                 ))    return do_mailqueue;
      if( !str_cmp( name, "do_monkey_gun" ) )
        return do_monkey_gun;
      if( !str_cmp( name, "do_minion" ) )
        return do_minion;
      if( !str_cmp( name, "do_makosen" ) )
        return do_makosen;
      if( !str_cmp( name, "do_mailroom" ) )
        return do_mailroom;
      if( !str_cmp( name, "do_maliceray" ) )
        return do_maliceray;
      if( !str_cmp( name, "do_mystic" ) )
        return do_mystic;
      if( !str_cmp( name, "do_mouth_cannon" ) )
        return do_mouth_cannon;
      if( !str_cmp( name, "do_mouth_blast" ) )
        return do_mouth_blast;
      if( !str_cmp( name, "do_make" ) )
        return do_make;
      if( !str_cmp( name, "do_makeboard" ) )
        return do_makeboard;
      if( !str_cmp( name, "do_majin" ) )
        return do_majin;
      if( !str_cmp( name, "do_makeclan" ) )
        return do_makeclan;
      if( !str_cmp( name, "do_makecouncil" ) )
        return do_makecouncil;
      if( !str_cmp( name, "do_makedeity" ) )
        return do_makedeity;
      if( !str_cmp( name, "do_makeplanet" ) )
        return do_makeplanet;
      if( !str_cmp( name, "do_manifestpotara" ) )
        return do_manifestpotara;
      if( !str_cmp( name, "do_makerepair" ) )
        return do_makerepair;
      if( !str_cmp( name, "do_makeship" ) )
        return do_makeship;
      if( !str_cmp( name, "do_makeshop" ) )
        return do_makeshop;
      if( !str_cmp( name, "do_makestarsystem" ) )
        return do_makestarsystem;
      if( !str_cmp( name, "do_makewizlist" ) )
        return do_makewizlist;
      if( !str_cmp( name, "do_mapout" ) )
        return do_mapout;
      if( !str_cmp( name, "do_marry" ) )
        return do_marry;
      if( !str_cmp( name, "do_masenko" ) )
        return do_masenko;
      if( !str_cmp( name, "do_massign" ) )
        return do_massign;
      if( !str_cmp( name, "do_mcreate" ) )
        return do_mcreate;
      if( !str_cmp( name, "do_mdelete" ) )
        return do_mdelete;
      if( !str_cmp( name, "do_meditate" ) )
        return do_meditate;
      if( !str_cmp( name, "do_members" ) )
        return do_members;
      if( !str_cmp( name, "do_memory" ) )
        return do_memory;
      if( !str_cmp( name, "do_mfind" ) )
        return do_mfind;
      if( !str_cmp( name, "do_minvoke" ) )
        return do_minvoke;
      if( !str_cmp( name, "do_mlist" ) )
        return do_mlist;
      if( !str_cmp( name, "do_mmenu" ) )
        return do_mmenu;
      if( !str_cmp( name, "do_morphcreate" ) )
        return do_morphcreate;
      if( !str_cmp( name, "do_morphdestroy" ) )
        return do_morphdestroy;
      if( !str_cmp( name, "do_morphset" ) )
        return do_morphset;
      if( !str_cmp( name, "do_morphstat" ) )
        return do_morphstat;
      if( !str_cmp( name, "do_mortalize" ) )
        return do_mortalize;
      if( !str_cmp( name, "do_mount" ) )
        return do_mount;
      if( !str_cmp( name, "do_mpshowbot" ) )
        return do_mpshowbot;
      if( !str_cmp( name, "do_mp_close_passage" ) )
        return do_mp_close_passage;
      if( !str_cmp( name, "do_mp_damage" ) )
        return do_mp_damage;
      if( !str_cmp( name, "do_mp_drain" ) )
        return do_mp_drain;
      if( !str_cmp( name, "do_mp_pl_damage" ) )
        return do_mp_pl_damage;
      if( !str_cmp( name, "do_mp_deposit" ) )
        return do_mp_deposit;
      if( !str_cmp( name, "do_mp_fill_in" ) )
        return do_mp_fill_in;
      if( !str_cmp( name, "do_mp_log" ) )
        return do_mp_log;
      if( !str_cmp( name, "do_mp_open_passage" ) )
        return do_mp_open_passage;
      if( !str_cmp( name, "do_mp_practice" ) )
        return do_mp_practice;
      if( !str_cmp( name, "do_mp_restore" ) )
        return do_mp_restore;
      if( !str_cmp( name, "do_mp_slay" ) )
        return do_mp_slay;
      if( !str_cmp( name, "do_mp_withdraw" ) )
        return do_mp_withdraw;
      // if ( !str_cmp( name, "do_mpadvance"                 ))    return do_mpadvance;
      if( !str_cmp( name, "do_mpapply" ) )
        return do_mpapply;
      if( !str_cmp( name, "do_mpapplyb" ) )
        return do_mpapplyb;
      if( !str_cmp( name, "do_mpasound" ) )
        return do_mpasound;
      if( !str_cmp( name, "do_mpasupress" ) )
        return do_mpasupress;
      if( !str_cmp( name, "do_mpat" ) )
        return do_mpat;
      if( !str_cmp( name, "do_mpbodybag" ) )
        return do_mpbodybag;
      if( !str_cmp( name, "do_mpcopy" ) )
        return do_mpcopy;
      if( !str_cmp( name, "do_mpdelay" ) )
        return do_mpdelay;
      if( !str_cmp( name, "do_mpdream" ) )
        return do_mpdream;
      if( !str_cmp( name, "do_mpecho" ) )
        return do_mpecho;
      if( !str_cmp( name, "do_mpechoaround" ) )
        return do_mpechoaround;
      if( !str_cmp( name, "do_mpechoat" ) )
        return do_mpechoat;
      if( !str_cmp( name, "do_mpechozone" ) )
        return do_mpechozone;
      if( !str_cmp( name, "do_mpedit" ) )
        return do_mpedit;
      if( !str_cmp( name, "do_mpfavor" ) )
        return do_mpfavor;
      if( !str_cmp( name, "do_mpfind" ) )
        return do_mpfind;
      if( !str_cmp( name, "do_mpforce" ) )
        return do_mpforce;
      if( !str_cmp( name, "do_mpgoto" ) )
        return do_mpgoto;
      if( !str_cmp( name, "do_mpinvis" ) )
        return do_mpinvis;
      if( !str_cmp( name, "do_mpjunk" ) )
        return do_mpjunk;
      if( !str_cmp( name, "do_mpkill" ) )
        return do_mpkill;
      if( !str_cmp( name, "do_mpmload" ) )
        return do_mpmload;
      if( !str_cmp( name, "do_mpmorph" ) )
        return do_mpmorph;
      if( !str_cmp( name, "do_mpmset" ) )
        return do_mpmset;
      if( !str_cmp( name, "do_mpmrppset" ) )
        return do_mpmrppset;
      if( !str_cmp( name, "do_mpmusic" ) )
        return do_mpmusic;
      if( !str_cmp( name, "do_mpmusicaround" ) )
        return do_mpmusicaround;
      if( !str_cmp( name, "do_mpmusicat" ) )
        return do_mpmusicat;
      if( !str_cmp( name, "do_mpnothing" ) )
        return do_mpnothing;
      if( !str_cmp( name, "do_mpoload" ) )
        return do_mpoload;
      if( !str_cmp( name, "do_mposet" ) )
        return do_mposet;
      if( !str_cmp( name, "do_mppardon" ) )
        return do_mppardon;
      if( !str_cmp( name, "do_mppeace" ) )
        return do_mppeace;
      if( !str_cmp( name, "do_mppkset" ) )
        return do_mppkset;
      if( !str_cmp( name, "do_mppldiv" ) )
        return do_mppldiv;
      if( !str_cmp( name, "do_mpplmult" ) )
        return do_mpplmult;
      if( !str_cmp( name, "do_mppurge" ) )
        return do_mppurge;
      if( !str_cmp( name, "do_mpscatter" ) )
        return do_mpscatter;
      if( !str_cmp( name, "do_mpsound" ) )
        return do_mpsound;
      if( !str_cmp( name, "do_mpsoundaround" ) )
        return do_mpsoundaround;
      if( !str_cmp( name, "do_mpsoundat" ) )
        return do_mpsoundat;
      if( !str_cmp( name, "do_mpstat" ) )
        return do_mpstat;
      if( !str_cmp( name, "do_mptransfer" ) )
        return do_mptransfer;
      if( !str_cmp( name, "do_mpunmorph" ) )
        return do_mpunmorph;
      if( !str_cmp( name, "do_mpunnuisance" ) )
        return do_mpunnuisance;
      if( !str_cmp( name, "do_mrange" ) )
        return do_mrange;
      if( !str_cmp( name, "do_mset" ) )
        return do_mset;
      if( !str_cmp( name, "do_mstat" ) )
        return do_mstat;
      if( !str_cmp( name, "do_multi_disk" ) )
        return do_multi_disk;
      if( !str_cmp( name, "do_multi_form" ) )
        return do_multi_form;
      if( !str_cmp( name, "do_multi_eye" ) )
        return do_multi_eye;
      if( !str_cmp( name, "do_murde" ) )
        return do_murde;
      if( !str_cmp( name, "do_murder" ) )
        return do_murder;
      if( !str_cmp( name, "do_muse" ) )
        return do_muse;
      if( !str_cmp( name, "do_music" ) )
        return do_music;
      if( !str_cmp( name, "do_mutate" ) )
        return do_mutate;
      if( !str_cmp( name, "do_mwhere" ) )
        return do_mwhere;
      if( !str_cmp( name, "do_mechaicer" ) )
        return do_mechaicer;
      if( !str_cmp( name, "do_mechanize" ) )
        return do_mechanize;
      break;
    case 'n':
      if( !str_cmp( name, "do_name" ) )
        return do_name;
      if( !str_cmp( name, "do_namekfuse" ) )
        return do_namekfuse;
      if( !str_cmp( name, "do_newbiechat" ) )
        return do_newbiechat;
      if( !str_cmp( name, "do_newbieset" ) )
        return do_newbieset;
      if( !str_cmp( name, "do_nephalimstorment" ) )
        return do_nephalimstorment;
      if( !str_cmp( name, "do_news" ) )
        return do_news;
#ifdef NEWSCORE
      if( !str_cmp( name, "do_newscore" ) )
        return do_newscore;
#endif
      if( !str_cmp( name, "do_negative_spirit_bomb" ) )
        return do_negative_spirit_bomb;
      if( !str_cmp( name, "do_newzones" ) )
        return do_newzones;
      if( !str_cmp( name, "do_noemote" ) )
        return do_noemote;
      if( !str_cmp( name, "do_noscream" ) )
        return do_noscream;
      if( !str_cmp( name, "do_nognote" ) )
        return do_nognote;
      if( !str_cmp( name, "do_nohelps" ) )
        return do_nohelps;
      if( !str_cmp( name, "do_noresolve" ) )
        return do_noresolve;
      if( !str_cmp( name, "do_north" ) )
        return do_north;
      if( !str_cmp( name, "do_northeast" ) )
        return do_northeast;
      if( !str_cmp( name, "do_northwest" ) )
        return do_northwest;
      if( !str_cmp( name, "do_notell" ) )
        return do_notell;
      if( !str_cmp( name, "do_noteroom" ) )
        return do_noteroom;
      if( !str_cmp( name, "do_notitle" ) )
        return do_notitle;
      if( !str_cmp( name, "do_novawave" ) )
        return do_novawave;
      if( !str_cmp( name, "do_nuisance" ) )
        return do_nuisance;
      if( !str_cmp( name, "do_nuisance" ) )
        return do_nuisance;
      break;
    case 'o':
      if( !str_cmp( name, "do_omega" ) )
        return do_omega;
        if (!str_cmp(name, "do_overdrive_mode")) return do_overdrive_mode;
      if( !str_cmp( name, "do_oassign" ) )
        return do_oassign;
      if( !str_cmp( name, "do_ocreate" ) )
        return do_ocreate;
      if( !str_cmp( name, "do_odelete" ) )
        return do_odelete;
      if( !str_cmp( name, "do_ofind" ) )
        return do_ofind;
      if( !str_cmp( name, "do_ofindtype" ) )
        return do_ofindtype;
      if( !str_cmp( name, "do_ogrub" ) )
        return do_ogrub;
      if( !str_cmp( name, "do_oinvoke" ) )
        return do_oinvoke;
      if( !str_cmp( name, "do_oldscore" ) )
        return do_oldscore;
      if( !str_cmp( name, "do_olist" ) )
        return do_olist;
      if( !str_cmp( name, "do_omenu" ) )
        return do_omenu;
      if( !str_cmp( name, "do_ooc" ) )
        return do_ooc;
      if( !str_cmp( name, "do_opcopy" ) )
        return do_opcopy;
      if( !str_cmp( name, "do_opedit" ) )
        return do_opedit;
      if( !str_cmp( name, "do_open" ) )
        return do_open;
      if( !str_cmp( name, "do_openhatch" ) )
        return do_openhatch;
      if( !str_cmp( name, "do_opfind" ) )
        return do_opfind;
      if( !str_cmp( name, "do_opstat" ) )
        return do_opstat;
      if( !str_cmp( name, "do_orange" ) )
        return do_orange;
      if( !str_cmp( name, "do_order" ) )
        return do_order;
      if( !str_cmp( name, "do_orders" ) )
        return do_orders;
      if( !str_cmp( name, "do_ordertalk" ) )
        return do_ordertalk;
      if( !str_cmp( name, "do_oset" ) )
        return do_oset;
      if( !str_cmp( name, "do_ostat" ) )
        return do_ostat;
      if( !str_cmp( name, "do_outcast" ) )
        return do_outcast;
      if( !str_cmp( name, "do_owhere" ) )
        return do_owhere;
      break;
    case 'p':
      if( !str_cmp( name, "do_pstatus" ) )
        return do_pstatus;
      if( !str_cmp( name, "do_pcquesting" ) )
        return do_pcquesting;
      if( !str_cmp( name, "do_pabsorb" ) )
        return do_pabsorb;
      if( !str_cmp( name, "do_page" ) )
        return do_page;
      if( !str_cmp( name, "do_pagelen" ) )
        return do_pagelen;
      if( !str_cmp( name, "do_pager" ) )
        return do_pager;
      if( !str_cmp( name, "do_pardon" ) )
        return do_pardon;
      if( !str_cmp( name, "do_partner" ) )
        return do_partner;
      if( !str_cmp( name, "do_password" ) )
        return do_password;
      if( !str_cmp( name, "do_pcrename" ) )
        return do_pcrename;
      if( !str_cmp( name, "do_pclastrename" ) )
        return do_pclastrename;
      if( !str_cmp( name, "do_peace" ) )
        return do_peace;
      if( !str_cmp( name, "do_pfiles" ) )
        return do_pfiles;
      if( !str_cmp( name, "do_pick" ) )
        return do_pick;
      if( !str_cmp( name, "do_pk" ) )
        return do_pk;
      if( !str_cmp( name, "do_planets" ) )
        return do_planets;
      if( !str_cmp(name, "do_pldock"))
	return do_pldock;
      if( !str_cmp( name, "do_plist" ) )
        return do_plist;
      if( !str_cmp( name, "do_plset" ) )
        return do_plset;
      if( !str_cmp( name, "do_pluogus" ) )
        return do_pluogus;
      if( !str_cmp( name, "do_poison_weapon" ) )
        return do_poison_weapon;
      if( !str_cmp( name, "do_potarafuse" ) )
        return do_potarafuse;
      if( !str_cmp( name, "do_powerdown" ) )
        return do_powerdown;
      if( !str_cmp( name, "do_powerup" ) )
        return do_powerup;
      if( !str_cmp( name, "do_practice" ) )
        return do_practice;
      if( !str_cmp( name, "do_privacy" ) )
        return do_privacy;
      if( !str_cmp( name, "do_project" ) )
        return do_project;
      if( !str_cmp( name, "do_promote" ) )
        return do_promote;
      if( !str_cmp( name, "do_prompt" ) )
        return do_prompt;
      if( !str_cmp( name, "do_pset" ) )
        return do_pset;
      if( !str_cmp( name, "do_psiblast" ) )
        return do_psiblast;
      if( !str_cmp( name, "do_pstat" ) )
        return do_pstat;
      if( !str_cmp( name, "do_pull" ) )
        return do_pull;
      if( !str_cmp( name, "do_punch" ) )
        return do_punch;
      if( !str_cmp( name, "do_puppet" ) )
        return do_puppet;
      if( !str_cmp( name, "do_purge" ) )
        return do_purge;
      if( !str_cmp( name, "do_push" ) )
        return do_push;
      if( !str_cmp( name, "do_put" ) )
        return do_put;
      if( !str_cmp( name, "do_pzap" ) )
        return do_pzap;
      break;
    case 'q':
      if( !str_cmp( name, "do_qpset" ) )
        return do_qpset;
      if( !str_cmp( name, "do_qpstat" ) )
        return do_qpstat;
      if( !str_cmp( name, "do_quaff" ) )
        return do_quaff;
      if( !str_cmp( name, "do_qui" ) )
        return do_qui;
      if( !str_cmp( name, "do_quit" ) )
        return do_quit;
      break;
    case 'r':
      if( !str_cmp( name, "do_raizou" ) )
        return do_raizou;
      if( !str_cmp( name, "do_raikyuu" ) )
        return do_raikyuu;
      if( !str_cmp( name, "do_riku" ) )
        return do_riku;
      if( !str_cmp( name, "do_roundhouse" ) )
        return do_roundhouse;
      if( !str_cmp( name, "do_railgun" ) )
        return do_railgun;
      if( !str_cmp( name, "do_renumber" ) )
        return do_renumber;
      if( !str_cmp( name, "do_revert_mystic" ) )
        return do_revert_mystic;
      if( !str_cmp( name, "do_roleplay" ) )
        return do_roleplay;
      if( !str_cmp( name, "do_reqbio" ) )
        return do_reqbio;
      if( !str_cmp( name, "do_resetalliances" ) )
        return do_resetalliances;
      if( !str_cmp( name, "do_racetalk" ) )
        return do_racetalk;
      if( !str_cmp( name, "do_radar" ) )
        return do_radar;
      if( !str_cmp( name, "do_rank" ) )
        return do_rank;
      if( !str_cmp( name, "do_rap" ) )
        return do_rap;
      if( !str_cmp( name, "do_rassign" ) )
        return do_rassign;
      if( !str_cmp( name, "do_rat" ) )
        return do_rat;
      // if ( !str_cmp( name, "do_rbeep"                     ))    return do_rbeep;
      // if ( !str_cmp( name, "do_rchannels"                 ))    return do_rchannels;
      // if ( !str_cmp( name, "do_rchanset"                  ))    return do_rchanset;
      // if ( !str_cmp( name, "do_rconnect"                  ))    return do_rconnect;
      if( !str_cmp( name, "do_rdelete" ) )
        return do_rdelete;
      // if ( !str_cmp( name, "do_rdisconnect"               ))    return do_rdisconnect;
      if( !str_cmp( name, "do_reboo" ) )
        return do_reboo;
      if( !str_cmp( name, "do_reboot" ) )
        return do_reboot;
      if( !str_cmp( name, "do_recharge" ) )
        return do_recharge;
      if( !str_cmp( name, "do_recho" ) )
        return do_recho;
      if( !str_cmp( name, "do_recall" ) )
        return do_recall;
      if( !str_cmp( name, "do_recite" ) )
        return do_recite;
      if( !str_cmp( name, "do_redit" ) )
        return do_redit;
      if( !str_cmp( name, "do_redraw_page" ) )
        return do_redraw_page;
     if( !str_cmp( name, "do_reflection" ) )
        return do_reflection;
      if( !str_cmp( name, "do_refresh_page" ) )
        return do_refresh_page;
      if( !str_cmp( name, "do_regoto" ) )
        return do_regoto;
      if( !str_cmp( name, "do_reimburse" ) )
        return do_reimburse;
      if( !str_cmp( name, "do_remains" ) )
        return do_remains;
      if( !str_cmp( name, "do_remove" ) )
        return do_remove;
      if( !str_cmp( name, "do_rempilot" ) )
        return do_rempilot;
      if( !str_cmp( name, "do_rent" ) )
        return do_rent;
      if( !str_cmp( name, "do_repair" ) )
        return do_repair;
      if( !str_cmp( name, "do_repairset" ) )
        return do_repairset;
      if( !str_cmp( name, "do_repairship" ) )
        return do_repairship;
      if( !str_cmp( name, "do_repairshops" ) )
        return do_repairshops;
      if( !str_cmp( name, "do_repairstat" ) )
        return do_repairstat;
      if( !str_cmp( name, "do_repeat" ) )
        return do_repeat;
      if( !str_cmp( name, "do_reply" ) )
        return do_reply;
      if( !str_cmp( name, "do_report" ) )
        return do_report;
      if( !str_cmp( name, "do_rescue" ) )
        return do_rescue;
      if( !str_cmp( name, "do_reserve" ) )
        return do_reserve;
      if( !str_cmp( name, "do_reset" ) )
        return do_reset;
      if( !str_cmp( name, "do_resetship" ) )
        return do_resetship;
      if( !str_cmp( name, "do_residence" ) )
        return do_residence;
      if( !str_cmp( name, "do_rest" ) )
        return do_rest;
      if( !str_cmp( name, "do_restore" ) )
        return do_restore;
      if( !str_cmp( name, "do_restoretime" ) )
        return do_restoretime;
      if( !str_cmp( name, "do_restrict" ) )
        return do_restrict;
      if( !str_cmp( name, "do_retell" ) )
        return do_retell;
      if( !str_cmp( name, "do_retire" ) )
        return do_retire;
      if( !str_cmp( name, "do_retran" ) )
        return do_retran;
      if( !str_cmp( name, "do_return" ) )
        return do_return;
      // if ( !str_cmp( name, "do_rfinger"                   ))    return do_rfinger;
      if( !str_cmp( name, "do_rgrub" ) )
        return do_rgrub;
      // if ( !str_cmp( name, "do_rignore"                   ))    return do_rignore;
      // if ( !str_cmp( name, "do_rinfo"                     ))    return do_rinfo;
      if( !str_cmp( name, "do_rings" ) )
        return do_rings;
      if( !str_cmp( name, "do_rip" ) )
        return do_rip;
      if( !str_cmp( name, "do_rlist" ) )
        return do_rlist;
      if( !str_cmp( name, "do_rmenu" ) )
        return do_rmenu;
      if( !str_cmp( name, "do_rocket_punch" ) )
        return do_rocket_punch;
      if( !str_cmp( name, "do_rolldie" ) )
        return do_rolldie;
      if( !str_cmp( name, "do_roster" ) )
        return do_roster;
      if( !str_cmp( name, "do_rpedit" ) )
        return do_rpedit;
      if( !str_cmp( name, "do_rpfind" ) )
        return do_rpfind;
      // if ( !str_cmp( name, "do_rping"                     ))    return do_rping;
      if( !str_cmp( name, "do_rppedit" ) )
        return do_rppedit;
      if( !str_cmp( name, "do_rpstat" ) )
        return do_rpstat;
      // if ( !str_cmp( name, "do_rquery"                    ))    return do_rquery;
      // if ( !str_cmp( name, "do_rreply"                    ))    return do_rreply;
      if( !str_cmp( name, "do_rreset" ) )
        return do_rreset;
      if( !str_cmp( name, "do_rset" ) )
        return do_rset;
      // if ( !str_cmp( name, "do_rsockets"                  ))    return do_rsockets;
      if( !str_cmp( name, "do_rstat" ) )
        return do_rstat;
      // if ( !str_cmp( name, "do_rtell"                     ))    return do_rtell;
      // if ( !str_cmp( name, "do_rwho"                      ))    return do_rwho;
      // if ( !str_cmp( name, "do_rwhois"                    ))    return do_rwhois;
      break;
    case 's':
      if( !str_cmp( name, "do_satellite_jump" ) )
        return do_satellite_jump;
     if( !str_cmp( name, "do_scan_transformation" ) )
        return do_scan_transformation;
      if( !str_cmp( name, "do_shakkahou" ) )
        return do_shakkahou;
      if( !str_cmp( name, "do_saibaman1" ) )
        return do_saibaman1;
      if( !str_cmp( name, "do_saibaman2" ) )
        return do_saibaman2;
      if( !str_cmp( name, "do_saibaman3" ) )
        return do_saibaman3;
      if( !str_cmp( name, "do_sent_chip" ) )
        return do_sent_chip;
      if( !str_cmp( name, "do_super_oozaru" ) )
        return do_super_oozaru;
      if( !str_cmp( name, "do_summon" ) )
        return do_summon;
      if( !str_cmp( name, "do_sanctuary" ) )
        return do_sanctuary;
      if( !str_cmp( name, "do_scattered_finger_beam" ) )
        return do_scattered_finger_beam;
      if( !str_cmp( name, "do_shockwave" ) )
        return do_shockwave;
      if( !str_cmp( name, "do_spirit_ball" ) )
        return do_spirit_ball;
      if( !str_cmp( name, "do_spirit_bomb" ) )
        return do_spirit_bomb;
      if( !str_cmp( name, "do_skga" ) )
        return do_skga;
      if( !str_cmp( name, "do_solar_flare" ) )
        return do_solar_flare;
      if( !str_cmp( name, "do_sacrifice" ) )
        return do_sacrifice;
      if( !str_cmp( name, "do_ss" ) )
        return do_ss;
      if( !str_cmp( name, "do_save" ) )
        return do_save;
      if( !str_cmp( name, "do_savealliances" ) )
        return do_savealliances;
      if( !str_cmp( name, "do_savearea" ) )
        return do_savearea;
      if( !str_cmp( name, "do_say" ) )
        return do_say;
      if( !str_cmp( name, "do_sbc" ) )
        return do_sbc;
      if( !str_cmp( name, "do_scan" ) )
        return do_scan;
      if( !str_cmp( name, "do_scatter" ) )
        return do_scatter;
      if( !str_cmp( name, "do_scatter_shot" ) )
        return do_scatter_shot;
      if( !str_cmp( name, "do_score" ) )
        return do_score;
      if( !str_cmp( name, "do_scribe" ) )
        return do_scribe;
      if( !str_cmp( name, "do_search" ) )
        return do_search;
      if( !str_cmp( name, "do_sedit" ) )
        return do_sedit;
      if( !str_cmp( name, "do_self_destruct" ) )
        return do_self_destruct;
      if( !str_cmp( name, "do_sell" ) )
        return do_sell;
      if( !str_cmp( name, "do_sellhouse" ) )
        return do_sellhouse;
      if( !str_cmp( name, "do_sellship" ) )
        return do_sellship;
      if( !str_cmp( name, "do_sense" ) )
        return do_sense;
      if( !str_cmp( name, "do_set_boot_time" ) )
        return do_set_boot_time;
      if( !str_cmp( name, "do_setage" ) )
        return do_setage;
      if( !str_cmp( name, "do_setclan" ) )
        return do_setclan;
      if( !str_cmp( name, "do_setclass" ) )
        return do_setclass;
      if( !str_cmp( name, "do_setcouncil" ) )
        return do_setcouncil;
      if( !str_cmp( name, "do_setdeity" ) )
        return do_setdeity;
      if( !str_cmp( name, "do_setheight" ) )
        return do_setheight;
      if( !str_cmp( name, "do_setplanet" ) )
        return do_setplanet;
      if( !str_cmp( name, "do_setrace" ) )
        return do_setrace;
      if( !str_cmp( name, "do_setship" ) )
        return do_setship;
      if( !str_cmp( name, "do_setstarsystem" ) )
        return do_setstarsystem;
      if( !str_cmp( name, "do_settime" ) )
        return do_settime;
      if( !str_cmp( name, "do_setrestoremessage" ) )
        return do_setrestoremessage;
      if( !str_cmp( name, "do_setweather" ) )
        return do_setweather;
      if( !str_cmp( name, "do_setweight" ) )
        return do_setweight;
      if( !str_cmp( name, "do_sflame1" ) )
        return do_sflame1;
      if( !str_cmp( name, "do_sflame2" ) )
        return do_sflame2;
      if( !str_cmp( name, "do_sflame3" ) )
        return do_sflame3;
      if( !str_cmp( name, "do_sharingan" ) )
        return do_sharingan;
      if( !str_cmp( name, "do_shieldbash" ) )
        return do_shieldbash;
      if( !str_cmp( name, "do_ships" ) )
        return do_ships;
      if( !str_cmp( name, "do_shipinfo" ) )
        return do_shipinfo;
      if( !str_cmp( name, "do_shops" ) )
        return do_shops;
      if( !str_cmp( name, "do_shopset" ) )
        return do_shopset;
      if( !str_cmp( name, "do_shopstat" ) )
        return do_shopstat;
      if( !str_cmp( name, "do_shout" ) )
        return do_shout;
      if( !str_cmp( name, "do_shove" ) )
        return do_shove;
      if( !str_cmp( name, "do_showrank" ) )
        return do_showrank;
      if( !str_cmp( name, "do_showclan" ) )
        return do_showclan;
      if( !str_cmp( name, "do_showclass" ) )
        return do_showclass;
      if( !str_cmp( name, "do_showcouncil" ) )
        return do_showcouncil;
      if( !str_cmp( name, "do_showdeity" ) )
        return do_showdeity;
      if( !str_cmp( name, "do_showimmlog" ) )
        return do_showimmlog;
      if( !str_cmp( name, "do_showlayers" ) )
        return do_showlayers;
      if( !str_cmp( name, "do_showplanet" ) )
        return do_showplanet;
      if( !str_cmp( name, "do_showrace" ) )
        return do_showrace;
      if( !str_cmp( name, "do_showship" ) )
        return do_showship;
      if( !str_cmp( name, "do_showstarsystem" ) )
        return do_showstarsystem;
      if( !str_cmp( name, "do_showweather" ) )
        return do_showweather;
      if( !str_cmp( name, "do_shutdow" ) )
        return do_shutdow;
      if( !str_cmp( name, "do_shutdown" ) )
        return do_shutdown;
      if( !str_cmp( name, "do_silence" ) )
        return do_silence;
      if( !str_cmp( name, "do_sit" ) )
        return do_sit;
      if( !str_cmp( name, "do_skin" ) )
        return do_skin;
      if( !str_cmp( name, "do_skin_trap" ) )
        return do_skin_trap;
      if( !str_cmp( name, "do_sla" ) )
        return do_sla;
      if( !str_cmp( name, "do_slay" ) )
        return do_slay;
      if( !str_cmp( name, "do_sleep" ) )
        return do_sleep;
      if( !str_cmp( name, "do_slice" ) )
        return do_slice;
      if( !str_cmp( name, "do_slist" ) )
        return do_slist;
      if( !str_cmp( name, "do_slookup" ) )
        return do_slookup;
      if( !str_cmp( name, "do_smoke" ) )
        return do_smoke;
      if( !str_cmp( name, "do_snoop" ) )
        return do_snoop;
      if( !str_cmp( name, "do_sober" ) )
        return do_sober;
      if( !str_cmp( name, "do_socials" ) )
        return do_socials;
      if( !str_cmp( name, "do_songofcreation" ) )
        return do_songofcreation;
      if( !str_cmp( name, "do_songoftime" ) )
        return do_songoftime;
      if( !str_cmp( name, "do_south" ) )
        return do_south;
      if( !str_cmp( name, "do_southeast" ) )
        return do_southeast;
      if( !str_cmp( name, "do_southwest" ) )
        return do_southwest;
      if( !str_cmp( name, "do_split_form" ) )
        return do_split_form;
      if( !str_cmp( name, "do_spar" ) )
        return do_spar;
      if( !str_cmp( name, "do_sparcheck" ) )
        return do_sparcheck;
      if( !str_cmp( name, "do_speak" ) )
        return do_speak;
      if( !str_cmp( name, "do_speeders" ) )
        return do_speeders;
      if( !str_cmp( name, "do_split" ) )
        return do_split;
      if( !str_cmp( name, "do_ssd_bomber" ) )
        return do_ssd_bomber;
      if( !str_cmp( name, "do_sset" ) )
        return do_sset;
      if( !str_cmp( name, "do_ssj" ) )
        return do_ssj;
      if( !str_cmp( name, "do_ssj2" ) )
        return do_ssj2;
      if( !str_cmp( name, "do_ssj3" ) )
        return do_ssj3;
        if( !str_cmp( name, "do_ssj4" ) )
          return do_ssj4;
          if( !str_cmp( name, "do_ssjgod" ) )
            return do_ssjgod;
      if( !str_cmp( name, "do_stand" ) )
        return do_stand;
      if( !str_cmp( name, "do_starsystems" ) )
        return do_starsystems;
      if( !str_cmp( name, "do_stat" ) )
        return do_stat;
      if( !str_cmp( name, "do_statreport" ) )
        return do_statreport;
      if( !str_cmp( name, "do_statshield" ) )
        return do_statshield;
      if( !str_cmp( name, "do_status" ) )
        return do_status;
      if( !str_cmp( name, "do_steal" ) )
        return do_steal;
      if( !str_cmp( name, "do_stopspar" ) )
        return do_stopspar;
      if( !str_cmp( name, "do_strew" ) )
        return do_strew;
      if( !str_cmp( name, "do_strip" ) )
        return do_strip;
      if( !str_cmp( name, "do_stun" ) )
        return do_stun;
      if( !str_cmp( name, "do_style" ) )
        return do_style;
      if( !str_cmp( name, "do_super_dragonfist" ) )
        return do_super_dragonfist;
      if( !str_cmp( name, "do_superandroid" ) )
        return do_superandroid;
      if( !str_cmp( name, "do_super_namek" ) )
        return do_super_namek;
      if( !str_cmp( name, "do_super_nova" ) )
        return do_super_nova;
      if( !str_cmp( name, "do_super_kamehameha" ) )
        return do_super_kamehameha;
      if( !str_cmp( name, "do_supplicate" ) )
        return do_supplicate;
      if( !str_cmp( name, "do_suppress" ) )
        return do_suppress;
      if( !str_cmp( name, "do_switch" ) )
        return do_switch;
      if( !str_cmp( name, "do_stripmajin" ) )
        return do_stripmajin;
      break;
    case 't':
      if( !str_cmp( name, "do_tslasher" ) )
        return do_tslasher;
      if( !str_cmp( name, "do_thunder_flash" ) )
	return do_thunder_flash;
      if( !str_cmp( name, "do_teleport_kamehameha" ) )
	return do_teleport_kamehameha;
      if( !str_cmp( name, "do_tuffle_blow" ) )
        return do_tuffle_blow;
      if( !str_cmp( name, "do_t4" ) )
        return do_t4;
      if( !str_cmp( name, "do_t3" ) )
        return do_t3;
      if( !str_cmp( name, "do_t2" ) )
        return do_t2;
      if( !str_cmp( name, "do_t1" ) )
        return do_t1;
      if( !str_cmp( name, "do_teachmajin" ) )
        return do_teachmajin;
      if( !str_cmp( name, "do_teachmystic" ) )
        return do_teachmystic;
      if( !str_cmp( name, "do_trap_ball" ) )
        return do_trap_ball;
      if( !str_cmp( name, "do_tribeam" ) )
        return do_tribeam;
      if( !str_cmp( name, "do_tri_form" ) )
        return do_tri_form;
      if( !str_cmp( name, "do_tag" ) )
        return do_tag;
      if( !str_cmp( name, "do_tamp" ) )
        return do_tamp;
      if( !str_cmp( name, "do_target" ) )
        return do_target;
      if( !str_cmp( name, "do_tell" ) )
        return do_tell;
      if( !str_cmp( name, "do_tentacle_attack" ) )
        return do_tentacle_attack;
      if( !str_cmp( name, "do_think" ) )
        return do_think;
      if( !str_cmp( name, "do_time" ) )
        return do_time;
      if( !str_cmp( name, "do_timecmd" ) )
        return do_timecmd;
      if( !str_cmp( name, "do_title" ) )
        return do_title;
      if( !str_cmp( name, "do_track" ) )
        return do_track;
      if( !str_cmp( name, "do_traffic" ) )
        return do_traffic;
      if( !str_cmp( name, "do_train" ) )
        return do_train;
      if( !str_cmp( name, "do_trajectory" ) )
        return do_trajectory;
      if( !str_cmp( name, "do_transfer" ) )
        return do_transfer;
      if( !str_cmp( name, "do_transform" ) )
        return do_transform;
//      if ( !str_cmp( name, "do_trust"                     ))    return do_trust;
      if( !str_cmp( name, "do_typo" ) )
        return do_typo;
      break;
    case 'u':
      if( !str_cmp( name, "do_uflame" ) )
        return do_uflame;
      if( !str_cmp( name, "do_undelete" ) )
	 return do_undelete;
      if( !str_cmp( name, "do_ultra_namek" ) )
        return do_ultra_namek;
      if( !str_cmp( name, "do_unbolt" ) )
        return do_unbolt;
      if( !str_cmp( name, "do_unbounty" ) )
        return do_unbounty;
      if( !str_cmp( name, "do_unfoldarea" ) )
        return do_unfoldarea;
      if( !str_cmp( name, "do_unhell" ) )
        return do_unhell;
      if( !str_cmp( name, "do_unlock" ) )
        return do_unlock;
      if( !str_cmp( name, "do_unnuisance" ) )
        return do_unnuisance;
      if( !str_cmp( name, "do_unsilence" ) )
        return do_unsilence;
      if( !str_cmp( name, "do_up" ) )
        return do_up;
      if( !str_cmp( name, "do_uppercut" ) )
        return do_uppercut;
      if( !str_cmp( name, "do_upgrade" ) )
        return do_upgrade;
      if( !str_cmp( name, "do_users" ) )
        return do_users;
      if( !str_cmp( name, "do_ussj" ) )
        return do_ussj;
      if( !str_cmp( name, "do_ussj2" ) )
        return do_ussj2;
      break;
    case 'v':
      if( !str_cmp( name, "do_vanishing_ball" ) )
        return do_vanishing_ball;
      if( !str_cmp( name, "do_vm1" ) )
        return do_vm1;
      if( !str_cmp( name, "do_vm2" ) )
        return do_vm2;
      if( !str_cmp( name, "do_value" ) )
        return do_value;
      if( !str_cmp( name, "do_vassign" ) )
        return do_vassign;
      if( !str_cmp( name, "do_version" ) )
        return do_version;
      if( !str_cmp( name, "do_victories" ) )
        return do_victories;
      if( !str_cmp( name, "do_viewbuf" ) )
        return do_viewbuf;
      if( !str_cmp( name, "do_visible" ) )
        return do_visible;
      if( !str_cmp( name, "do_vnums" ) )
        return do_vnums;
      if( !str_cmp( name, "do_vsearch" ) )
        return do_vsearch;
      break;
    case 'w':
      if( !str_cmp( name, "do_wake" ) )
        return do_wake;
      if( !str_cmp( name, "do_whirlwind_spin" ) )
        return do_whirlwind_spin;
      if( !str_cmp( name, "do_wog" ) )
        return do_wog;
      if( !str_cmp( name, "do_wff" ) )
        return do_wff;
      if( !str_cmp( name, "do_warn" ) )
        return do_warn;
      if( !str_cmp( name, "do_wartalk" ) )
        return do_wartalk;
      if( !str_cmp( name, "do_watch" ) )
        return do_watch;
      if( !str_cmp( name, "do_wear" ) )
        return do_wear;
      if( !str_cmp( name, "do_weather" ) )
        return do_weather;
      if( !str_cmp( name, "do_west" ) )
        return do_west;
      if( !str_cmp( name, "do_where" ) )
        return do_where;
      if( !str_cmp( name, "do_whisper" ) )
        return do_whisper;
      if( !str_cmp( name, "do_who" ) )
        return do_who;
      if ( !str_cmp( name, "do_whois" ) )   
        return do_whois;
      if( !str_cmp( name, "do_wimpy" ) )
        return do_wimpy;
      if( !str_cmp( name, "do_wizhelp" ) )
        return do_wizhelp;
      if( !str_cmp( name, "do_wizinfo" ) )
        return do_wizinfo;
      if( !str_cmp( name, "do_wizlist" ) )
        return do_wizlist;
      if( !str_cmp( name, "do_wizlock" ) )
        return do_wizlock;
      if( !str_cmp( name, "do_worth" ) )
        return do_worth;
      if( !str_cmp( name, "do_wss" ) )
        return do_wss;
      break;
    case 'x':
      if( !str_cmp( name, "do_xbuster" ) )
        return do_xbuster;
      break;
    case 'y':
      if( !str_cmp( name, "do_yell" ) )
        return do_yell;
      break;
    case 'z':
      if( !str_cmp( name, "do_zap" ) )
        return do_zap;
      if( !str_cmp( name, "do_zones" ) )
        return do_zones;
      if( !str_cmp( name, "do_zoramask" ) )
        return do_zoramask;
      if( !str_cmp( name, "do_zsaber" ) )
        return do_zsaber;
      break;
/*//T2*/
  }
  return skill_notfound;
}

char *spell_name( SPELL_FUN * spell )
{
  if( spell == spell_smaug )
    return "spell_smaug";

  if( spell == spell_null )
    return "spell_null";
  if( spell == spell_sensu_bean )
    return "spell_sensu_bean";
  return "reserved";
}

char *skill_name( DO_FUN * skill )
{
  static char buf[64];
/*//T3*/

  if( skill == do_devil_flash )
    return "do_devil_flash";
  if( skill == do_dragonballscatter )
    return "do_dragonballscatter";
  if( skill == do_acid_burn )
    return "do_acid_burn";
  if( skill == do_acid_slash )
    return "do_acid_slash";
  if( skill == do_fakemoon )
    return "do_fakemoon";
  if( skill == do_fusiondance )
    return "do_fusiondance";
  if( skill == do_defuse )
    return "do_defuse";
  if( skill == do_clawstrike )
    return "do_clawstrike";
  if( skill == do_aligncheck )
    return "do_aligncheck"; 
  if( skill == do_aassign )
    return "do_aassign";
  if( skill == do_absorb )
    return "do_absorb";
  if( skill == do_accelerate )
    return "do_accelerate";
  if( skill == do_accessories )
    return "do_accessories";
  if(skill == do_abandonrank)
    return "do_abandonrank";
  if( skill == do_add_imm_host )
    return "do_add_imm_host";
  if( skill == do_addpilot )
    return "do_addpilot";
  if( skill == do_addlessj )
    return "do_addlessj";
  if( skill == do_admintalk )
    return "do_admintalk";
  if( skill == do_adminto )
    return "do_adminto";
  if( skill == do_advance )
    return "do_advance";
  if( skill == do_affected )
    return "do_affected";
  if( skill == do_afk )
    return "do_afk";
  if( skill == do_ahelp )
    return "do_ahelp";
  if( skill == do_aid )
    return "do_aid";
  if( skill == do_alist )
    return "do_alist";
  if( skill == do_allow )
    return "do_allow";
  if( skill == do_allhigh )
    return "do_allhigh";
  if( skill == do_allships )
    return "do_allships";
  if( skill == do_allspeeders )
    return "do_allspeeders";
  if( skill == do_androidfuse )
    return "do_androidfuse";
  if( skill == do_analyze )
    return "do_analyze";
  if( skill == do_ansi )
    return "do_ansi";
  if( skill == do_answer )
    return "do_answer";
  if( skill == do_autotrain )
    return "do_autotrain";
  if( skill == do_apply )
    return "do_apply";
  if( skill == do_appraise )
    return "do_appraise";
  if( skill == do_areas )
    return "do_areas";
  if( skill == do_aset )
    return "do_aset";
  if( skill == do_ask )
    return "do_ask";
  if( skill == do_astat )
    return "do_astat";
  if( skill == do_at )
    return "do_at";
  if( skill == do_atobj )
    return "do_atobj";
  if( skill == do_auction )
    return "do_auction";
  if( skill == do_aura_set )
    return "do_aura_set";
  if( skill == do_authorize )
    return "do_authorize";
  if( skill == do_autopilot )
    return "do_autopilot";
  if( skill == do_autorecharge )
    return "do_autorecharge";
  if( skill == do_autotrack )
    return "do_autotrack";
  if( skill == do_avtalk )
    return "do_avtalk";
  if( skill == do_backup )
    return "do_backup";
  if( skill == do_battery )
    return "do_battery";
  if( skill == do_blast_zone )
    return "do_blast_zone";
  if( skill == do_banish )
    return "do_banish";
  if( skill == do_big_bang )
    return "do_big_bang";
  if( skill == do_burning_attack )
    return "do_burning_attack";
  if( skill == do_drp )
    return "do_drp";
  if( skill == do_doublekick )
    return "do_doublekick";
  if( skill == do_doublepunch )
    return "do_doublepunch";
  if( skill == do_darkness_flare )
    return "do_darkness_flare";
  if( skill == do_demonic_eclipse )
    return "do_demonic_eclipse";
  if( skill == do_destructive_wave )
    return "do_destructive_wave";
  if( skill == do_divinewrath )
    return "do_divinewrath";
  if( skill == do_dodon_ray )
    return "do_dodon_ray";
  if( skill == do_desperation )
    return "do_desperation";
  if( skill == do_finalshine )
    return "do_finalshine";
  if( skill == do_finishing_buster )
    return "do_finishing_buster";
  if( skill == do_gallic_gun )
    return "do_gallic_gun";
  if( skill == do_gained )
    return "do_gained";
  if( skill == do_grant )
    return "do_grant";
  if( skill == do_heaven_splitter_cannon )
    return "do_heaven_splitter_cannon";
  if( skill == do_hellzone_grenade )
    return "do_hellzone_grenade";
  if( skill == do_monkey_gun )
    return "do_monkey_gun";
  if( skill == do_majin )
    return "do_majin";
  if( skill == do_minion )
    return "do_minion";
  if( skill == do_makosen )
    return "do_makosen";
  if( skill == do_maliceray )
    return "do_maliceray";
  if( skill == do_sent_chip )
    return "do_sent_chip";
  if( skill == do_super_oozaru )
    return "do_super_oozaru";
  if( skill == do_ss )
    return "do_ss";
  if( skill == do_sanctuary )
    return "do_sanctuary";
  if( skill == do_superandroid )
    return "do_superandroid";
  if( skill == do_super_dragonfist )
    return "do_super_dragonfist";
  if( skill == do_scattered_finger_beam )
    return "do_scattered_finger_beam";
  if( skill == do_shockwave )
    return "do_shockwave";
  if( skill == do_spirit_ball )
    return "do_spirit_ball";
  if( skill == do_negative_spirit_bomb )
    return "do_negative_spirit_bomb";
  if( skill == do_spirit_bomb )
    return "do_spirit_bomb";
  if( skill == do_summon )
    return "do_summon";
  if( skill == do_thunder_flash )
    return "do_thunder_flash";
  if( skill == do_teachmystic )
    return "do_teachmystic";
  if( skill == do_teachmajin )
    return "do_teachmajin";
  if( skill == do_stripmajin )
    return "do_stripmajin";
  if( skill == do_trap_ball )
    return "do_trap_ball";
  if( skill == do_backstab )
    return "do_backstab";
  if( skill == do_balzhur )
    return "do_balzhur";
  if( skill == do_bamfin )
    return "do_bamfin";
  if( skill == do_bamfout )
    return "do_bamfout";
  if( skill == do_ban )
    return "do_ban";
  if( skill == do_bash )
    return "do_bash";
  if( skill == do_bashdoor )
    return "do_bashdoor";
  if( skill == do_bbk )
    return "do_bbk";
  if( skill == do_beep )
    return "do_beep";
  if( skill == do_berserk )
    return "do_berserk";
  if( skill == do_bestow )
    return "do_bestow";
  if( skill == do_bestowarea )
    return "do_bestowarea";
  if( skill == do_bio )
    return "do_bio";
  if( skill == do_bite )
    return "do_bite";
  if( skill == do_bloodlet )
    return "do_bloodlet";
  if( skill == do_bless )
    return "do_bless";
  if( skill == do_board )
    return "do_board";
  if( skill == do_boards )
    return "do_boards";
  if( skill == do_bodybag )
    return "do_bodybag";
  if( skill == do_bolt )
    return "do_bolt";
  if( skill == do_botwatch )
    return "do_botwatch";
  if( skill == do_bounty )
    return "do_bounty";
  if( skill == do_brandish )
    return "do_brandish";
  if( skill == do_brew )
    return "do_brew";
  if( skill == do_bset )
    return "do_bset";
  if( skill == do_bstat )
    return "do_bstat";
  if( skill == do_bug )
    return "do_bug";
  if( skill == do_bury )
    return "do_bury";
  if( skill == do_buy )
    return "do_buy";
//  if( skill == do_bank )
//    return "do_bank";
  if( skill == do_buyship )
    return "do_buyship";
  if( skill == do_claimrank )
    return "do_claimrank";
  if( skill == do_cruise_punch )
    return "do_cruise_punch";
  if( skill == do_callship )
    return "do_callship";
  if( skill == do_calculate )
    return "do_calculate";
  if( skill == do_cache )
    return "do_cache";
  if( skill == do_candy_blast )
    return "do_candy_blast";
  if( skill == do_cast )
    return "do_cast";
  if( skill == do_cedit )
    return "do_cedit";
  if( skill == do_censor )
    return "do_censor";
  if( skill == do_chaff )
    return "do_chaff";
  if( skill == do_chaos )
    return "do_chaos";
  if( skill == do_charge )
    return "do_charge";
  if( skill == do_channels )
    return "do_channels";
  if( skill == do_chat )
    return "do_chat";
  if( skill == do_check_vnums )
    return "do_check_vnums";
  if( skill == do_check_ld )
    return "do_check_ld";
  if( skill == do_circle )
    return "do_circle";
  if( skill == do_clanadmin )
    return "do_clanadmin";
  if( skill == do_clanrecall )
    return "do_clanrecall";
  if( skill == do_clandeposit )
    return "do_clandeposit";
  if( skill == do_clanwithdraw )
    return "do_clanwithdraw";
  if( skill == do_claninfo )
    return "do_claninfo";
  if( skill == do_clans )
    return "do_clans";
  if( skill == do_clantalk )
    return "do_clantalk";
  if( skill == do_cleaver )
    return "do_cleaver";
  if( skill == do_clear )
    return "do_clear";
  if( skill == do_clearhiscore )
    return "do_clearhiscore";
  if( skill == do_climate )
    return "do_climate";
  if( skill == do_climb )
    return "do_climb";
  if( skill == do_clone )
    return "do_clone";
  if( skill == do_clones )
    return "do_clones";
  if( skill == do_close )
    return "do_close";
  if( skill == do_closehatch )
    return "do_closehatch";
  if( skill == do_cmdtable )
    return "do_cmdtable";
  if( skill == do_cmenu )
    return "do_cmenu";
  if( skill == do_colorize )
    return "do_colorize";
  if( skill == do_corpse )
    return "do_corpse";
  if( skill == do_combat )
    return "do_combat";
  if( skill == do_commands )
    return "do_commands";
  if( skill == do_comment )
    return "do_comment";
  if( skill == do_compare )
    return "do_compare";
  if( skill == do_chou_kamehameha )
    return "do_chou_kamehameha";
#ifdef MCCP
  if( skill == do_compress )
    return "do_compress";
#endif
  if( skill == do_config )
    return "do_config";
  if( skill == do_connect )
    return "do_connect";
  if( skill == do_consider )
    return "do_consider";
  if( skill == do_cook )
    return "do_cook";
  if( skill == do_copyship )
    return "do_copyship";
  if( skill == do_council_induct )
    return "do_council_induct";
  if( skill == do_council_outcast )
    return "do_council_outcast";
  if( skill == do_councils )
    return "do_councils";
  if( skill == do_counciltalk )
    return "do_counciltalk";
  if( skill == do_credits )
    return "do_credits";
  if( skill == do_cset )
    return "do_cset";
  if( skill == do_dd )
    return "do_dd";
  if( skill == do_ddd )
    return "do_ddd";
  if( skill == do_deathbeam )
    return "do_deathbeam";
  if( skill == do_daichiretsuzan )
    return "do_daichiretsuzan";
  if( skill == do_death_ball )
    return "do_death_ball";
  if( skill == do_deities )
    return "do_deities";
  if( skill == do_delay )
    return "do_delay";
  if( skill == do_delete )
    return "do_delete";
  if( skill == do_demonweapon )
    return "do_demonweapon";
  if( skill == do_demon_wave )
    return "do_demon_wave";
  if( skill == do_deny )
    return "do_deny";
  if( skill == do_description )
    return "do_description";
  if( skill == do_description1 )
    return "do_description1";
  if( skill == do_description2 )
    return "do_description2";
  if( skill == do_description3 )
    return "do_description3";
  if( skill == do_description4 )
    return "do_description4";
  if( skill == do_description5 )
    return "do_description5";
  if( skill == do_destro )
    return "do_destro";
  if( skill == do_destroy )
    return "do_destroy";
  if( skill == do_detrap )
    return "do_detrap";
  if( skill == do_devote )
    return "do_devote";
  if( skill == do_diagnose )
    return "do_diagnose";
  if( skill == do_dig )
    return "do_dig";
  if( skill == do_disarm )
    return "do_disarm";
  if( skill == do_disconnect )
    return "do_disconnect";
  if( skill == do_dismiss )
    return "do_dismiss";
  if( skill == do_dismount )
    return "do_dismount";
  if( skill == do_divorce )
    return "do_divorce";
  if( skill == do_dmesg )
    return "do_dmesg";
  if( skill == do_dnd )
    return "do_dnd";
  if( skill == do_dnsusers )
    return "do_dnsusers";
  if( skill == do_down )
    return "do_down";
  if( skill == do_drag )
    return "do_drag";
  if( skill == do_drink )
    return "do_drink";
  if( skill == do_drive )
    return "do_drive";
  if( skill == do_drop )
    return "do_drop";
  if( skill == do_duplicate )
    return "do_duplicate";
  if( skill == do_buuball )
    return "do_buuball";
  if( skill == do_earth_line_blast )
    return "do_earth_line_blast";
  if( skill == do_enhance )
    return "do_enhance";
  if( skill == do_eliminate )
    return "do_eliminate";
  if( skill == do_east )
    return "do_east";
  if( skill == do_eat )
    return "do_eat";
  if( skill == do_ech )
    return "do_ech";
  if( skill == do_echo )
    return "do_echo";
  if( skill == do_ehb )
    return "do_ehb";
  if( skill == do_electric_shield )
    return "do_electric_shield";
  if( skill == do_elevate )
    return "do_elevate";
  if( skill == do_email )
    return "do_email";
  if( skill == do_emote )
    return "do_emote";
  if( skill == do_empty )
    return "do_empty";
  if( skill == do_energy_ball )
    return "do_energy_ball";
  if( skill == do_energy_ring )
    return "do_energy_ring";
  if( skill == do_enter )
    return "do_enter";
  if( skill == do_equipment )
    return "do_equipment";
  if( skill == do_evilboost )
    return "do_evilboost";
  if( skill == do_evilsurge )
    return "do_evilsurge";
  if( skill == do_eviloverload )
    return "do_eviloverload";
  if( skill == do_examine )
    return "do_examine";
  if( skill == do_exits )
    return "do_exits";
  if( skill == do_eye_beam )
    return "do_eye_beam";
  if( skill == do_evolve )
    return "do_evolve";
  if( skill == do_mutate )
    return "do_mutate";
  if( skill == do_extreme )
    return "do_extreme";
  if( skill == do_essj )
    return "do_essj";
  if( skill == do_hssj )
    return "do_hssj";
  if( skill == do_feed )
    return "do_feed";
  if( skill == do_ff )
    return "do_ff";
  if( skill == do_fill )
    return "do_fill";
  if( skill == do_findnote )
    return "do_findnote";
  if( skill == do_finger )
    return "do_finger";
  if( skill == do_finger_beam )
    return "do_finger_beam";
  if( skill == do_fire )
    return "do_fire";
  if( skill == do_fireball )
    return "do_fireball";
  if( skill == do_fixchar )
    return "do_fixchar";
  if( skill == do_fixed )
    return "do_fixed";
  if( skill == do_flee )
    return "do_flee";
  if( skill == do_fly )
    return "do_fly";
  if( skill == do_foldarea )
    return "do_foldarea";
  if( skill == do_follow )
    return "do_follow";
  if( skill == do_for )
    return "do_for";
  if( skill == do_force )
    return "do_force";
  if( skill == do_forceclose )
    return "do_forceclose";
  if( skill == do_form_password )
    return "do_form_password";
  if( skill == do_fos )
    return "do_fos";
  if( skill == do_fprompt )
    return "do_fprompt";
  if( skill == do_fquit )
    return "do_fquit";
  if( skill == do_freeze )
    return "do_freeze";
  if( skill == do_fshow )
    return "do_fshow";
  if( skill == do_fslay )
    return "do_fslay";
  if( skill == do_gaso )
    return "do_gaso";
  if( skill == do_get )
    return "do_get";
  if( skill == do_genocide_attack )
    return "do_genocide_attack";
  if( skill == do_gfighting )
    return "do_gfighting";
  if( skill == do_ghost )
    return "do_ghost";
  if( skill == do_give )
    return "do_give";
  if( skill == do_giant_size )
    return "do_giant_size";
  if( skill == do_glance )
    return "do_glance";
  if( skill == do_global_boards )
    return "do_global_boards";
  if( skill == do_global_note )
    return "do_global_note";
  if( skill == do_godset )
    return "do_godset";
  if( skill == do_godstat )
    return "do_godstat";
  if( skill == do_godwho )
    return "do_godwho";
  if( skill == do_gohome )
    return "do_gohome";
  if( skill == do_gold )
    return "do_gold";
  if( skill == do_goto )
    return "do_goto";
  if( skill == do_gouge )
    return "do_gouge";
  if( skill == do_group )
    return "do_group";
  if( skill == do_growth )
    return "do_growth";
  if( skill == do_grub )
    return "do_grub";
  if( skill == do_gtell )
    return "do_gtell";
  if( skill == do_guilds )
    return "do_guilds";
  if( skill == do_guildtalk )
    return "do_guildtalk";
  if( skill == do_gwhere )
    return "do_gwhere";
  if( skill == do_hcchat )
    return "do_hcchat";
  if( skill == do_hyper )
    return "do_hyper";
  if( skill == do_hedit )
    return "do_hedit";
  if( skill == do_hell )
    return "do_hell";
  if( skill == do_hells_flash )
    return "do_hells_flash";
  if( skill == do_help )
    return "do_help";
  if( skill == do_hide )
    return "do_hide";
  if( skill == do_hiscore )
    return "do_hiscore";
  if( skill == do_hiscoset )
    return "do_hiscoset";
  if( skill == do_hitall )
    return "do_hitall";
  if( skill == do_hl )
    return "do_hl";
  if( skill == do_hlist )
    return "do_hlist";
  if( skill == do_holylight )
    return "do_holylight";
  if( skill == do_homebuy )
    return "do_homebuy";
  if( skill == do_homepage )
    return "do_homepage";
  if( skill == do_honoo )
    return "do_honoo";
  if( skill == do_hotboot )
    return "do_hotboot";
  if( skill == do_house )
    return "do_house";
  if( skill == do_hset )
    return "do_hset";
  if( skill == do_hyperspace )
    return "do_hyperspace";
  if( skill == do_icer_transform_2 )
    return "do_icer_transform_2";
  if( skill == do_icer_transform_3 )
    return "do_icer_transform_3";
  if( skill == do_instant_trans )
    return "do_instant_trans";
  if( skill == do_icer_transform_4 )
    return "do_icer_transform_4";
  if( skill == do_icer_transform_5 )
    return "do_icer_transform_5";
  // if ( skill == do_ichannels                   )    return "do_ichannels";
  // if ( skill == do_icommand                    )    return "do_icommand";
  if( skill == do_icq_number )
    return "do_icq_number";
  if( skill == do_ide )
    return "do_ide";
  if( skill == do_idea )
    return "do_idea";
  if( skill == do_ignore )
    return "do_ignore";
  // if ( skill == do_ilist                       )    return "do_ilist";
  // if ( skill == do_imc                         )    return "do_imc";
  // if ( skill == do_imclist                     )    return "do_imclist";
  if( skill == do_imm_morph )
    return "do_imm_morph";
  if( skill == do_imm_unmorph )
    return "do_imm_unmorph";
  if( skill == do_immtalk )
    return "do_immtalk";
  if( skill == do_immortalaurareset )
    return "do_immortalaurareset";
  if( skill == do_incognito )
    return "do_incognito";
  if( skill == do_induct )
    return "do_induct";
  if( skill == do_info )
    return "do_info";
  if( skill == do_integrate )
    return "do_integrate";
  if( skill == do_mechanize )
    return "do_mechanize";
  if( skill == do_installarea )
    return "do_installarea";
  if( skill == do_instaroom )
    return "do_instaroom";
  if( skill == do_instazone )
    return "do_instazone";
  if( skill == do_inventory )
    return "do_inventory";
  if( skill == do_invis )
    return "do_invis";
  if( skill == do_ipcompare )
    return "do_ipcompare";
  // if ( skill == do_isetup                      )    return "do_isetup";
  // if ( skill == do_istats                      )    return "do_istats";
  if( skill == do_joinclan )
    return "do_joinclan";
  if( skill == do_jibaku )
    return "do_jibaku";
  if( skill == do_kaiocreate )
    return "do_kaiocreate";
  if( skill == do_kakusandan )
    return "do_kakusandan";
  if( skill == do_kaioken )
    return "do_kaioken";
  if( skill == do_kairestore )
    return "do_kairestore";
  if( skill == do_shakkahou )
    return "do_shakkahou";
  if( skill == do_raizou )
    return "do_raizou";
  if( skill == do_raikyuu )
    return "do_raikyuu";
  if( skill == do_riku )
    return "do_riku";
  if( skill == do_kamehameha )
    return "do_kamehameha";
  if( skill == do_khistory )
    return "do_khistory";
  if( skill == do_kick )
    return "do_kick";
  if( skill == do_kill )
    return "do_kill";
  if( skill == do_ki_burst )
    return "do_ki_burst";
  if( skill == do_ki_heal )
    return "do_ki_heal";
  if( skill == do_land )
    return "do_land";
  if( skill == do_languages )
    return "do_languages";
  if( skill == do_last )
    return "do_last";
  if( skill == do_launch )
    return "do_launch";
  if( skill == do_laws )
    return "do_laws";
  if( skill == do_leave )
    return "do_leave";
  if( skill == do_leaveship )
    return "do_leaveship";
  if( skill == do_level )
    return "do_level";
  if( skill == do_light )
    return "do_light";
  if( skill == do_list )
    return "do_list";
  if( skill == do_litterbug )
    return "do_litterbug";
  if( skill == do_loadarea )
    return "do_loadarea";
  if( skill == do_loadup )
    return "do_loadup";
  if( skill == do_lock )
    return "do_lock";
  if( skill == do_log )
    return "do_log";
  if( skill == do_look )
    return "do_look";
  if( skill == do_lookmap )
    return "do_lookmap";
  if( skill == do_loop )
    return "do_loop";
  if( skill == do_low_purge )
    return "do_low_purge";
  if( skill == do_lssj )
    return "do_lssj";
  // if ( skill == do_mailqueue                   )    return "do_mailqueue";
  if( skill == do_mystic )
    return "do_mystic";
  if( skill == do_mouth_cannon )
    return "do_mouth_cannon";
  if( skill == do_mouth_blast )
    return "do_mouth_blast";
  if( skill == do_mailroom )
    return "do_mailroom";
  if( skill == do_make )
    return "do_make";
  if( skill == do_makeboard )
    return "do_makeboard";
  if( skill == do_makeclan )
    return "do_makeclan";
  if( skill == do_makecouncil )
    return "do_makecouncil";
  if( skill == do_makedeity )
    return "do_makedeity";
  if( skill == do_makeplanet )
    return "do_makeplanet";
  if( skill == do_makerepair )
    return "do_makerepair";
  if( skill == do_makeship )
    return "do_makeship";
  if( skill == do_makeshop )
    return "do_makeshop";
  if( skill == do_makestarsystem )
    return "do_makestarsystem";
  if( skill == do_makewizlist )
    return "do_makewizlist";
  if( skill == do_mapout )
    return "do_mapout";
  if( skill == do_marry )
    return "do_marry";
  if( skill == do_masenko )
    return "do_masenko";
  if( skill == do_massign )
    return "do_massign";
  if( skill == do_mcreate )
    return "do_mcreate";
  if( skill == do_mdelete )
    return "do_mdelete";
  if( skill == do_meditate )
    return "do_meditate";
  if( skill == do_members )
    return "do_members";
  if( skill == do_memory )
    return "do_memory";
  if( skill == do_mfind )
    return "do_mfind";
  if( skill == do_minvoke )
    return "do_minvoke";
  if( skill == do_mlist )
    return "do_mlist";
  if( skill == do_mmenu )
    return "do_mmenu";
  if( skill == do_morphcreate )
    return "do_morphcreate";
  if( skill == do_morphdestroy )
    return "do_morphdestroy";
  if( skill == do_morphset )
    return "do_morphset";
  if( skill == do_morphstat )
    return "do_morphstat";
  if( skill == do_mortalize )
    return "do_mortalize";
  if( skill == do_mount )
    return "do_mount";
  if( skill == do_mpshowbot )
    return "do_mpshowbot";
  if( skill == do_mp_close_passage )
    return "do_mp_close_passage";
  if( skill == do_mp_damage )
    return "do_mp_damage";
  if( skill == do_mp_drain )
    return "do_mp_drain";
  if( skill == do_mp_pl_damage )
    return "do_mp_pl_damage";
  if( skill == do_mp_deposit )
    return "do_mp_deposit";
  if( skill == do_mp_fill_in )
    return "do_mp_fill_in";
  if( skill == do_mp_log )
    return "do_mp_log";
  if( skill == do_mp_open_passage )
    return "do_mp_open_passage";
  if( skill == do_mp_practice )
    return "do_mp_practice";
  if( skill == do_mp_restore )
    return "do_mp_restore";
  if( skill == do_mp_slay )
    return "do_mp_slay";
  if( skill == do_mp_withdraw )
    return "do_mp_withdraw";
  // if ( skill == do_mpadvance                   )    return "do_mpadvance";
  if( skill == do_mpapply )
    return "do_mpapply";
  if( skill == do_mpapplyb )
    return "do_mpapplyb";
  if( skill == do_mpasound )
    return "do_mpasound";
  if( skill == do_mpasupress )
    return "do_mpasupress";
  if( skill == do_mpat )
    return "do_mpat";
  if( skill == do_mpbodybag )
    return "do_mpbodybag";
  if( skill == do_mpcopy )
    return "do_mpcopy";
  if( skill == do_mpdelay )
    return "do_mpdelay";
  if( skill == do_mpdream )
    return "do_mpdream";
  if( skill == do_mpecho )
    return "do_mpecho";
  if( skill == do_mpechoaround )
    return "do_mpechoaround";
  if( skill == do_mpechoat )
    return "do_mpechoat";
  if( skill == do_mpechozone )
    return "do_mpechozone";
  if( skill == do_mpedit )
    return "do_mpedit";
  if( skill == do_mpfavor )
    return "do_mpfavor";
  if( skill == do_mpfind )
    return "do_mpfind";
  if( skill == do_mpforce )
    return "do_mpforce";
  if( skill == do_mpgoto )
    return "do_mpgoto";
  if( skill == do_mpinvis )
    return "do_mpinvis";
  if( skill == do_mpjunk )
    return "do_mpjunk";
  if( skill == do_mpkill )
    return "do_mpkill";
  if( skill == do_mpmload )
    return "do_mpmload";
  if( skill == do_mpmorph )
    return "do_mpmorph";
  if( skill == do_mpmset )
    return "do_mpmset";
  if( skill == do_mpmrppset )
    return "do_mpmrppset";
  if( skill == do_mpmusic )
    return "do_mpmusic";
  if( skill == do_mpmusicaround )
    return "do_mpmusicaround";
  if( skill == do_mpmusicat )
    return "do_mpmusicat";
  if( skill == do_mpnothing )
    return "do_mpnothing";
  if( skill == do_mpnuisance )
    return "do_mpnuisance";
  if( skill == do_mpoload )
    return "do_mpoload";
  if( skill == do_mposet )
    return "do_mposet";
  if( skill == do_mppardon )
    return "do_mppardon";
  if( skill == do_mppeace )
    return "do_mppeace";
  if( skill == do_mppkset )
    return "do_mppkset";
  if( skill == do_mppldiv )
    return "do_mppldiv";
  if( skill == do_mpplmult )
    return "do_mpplmult";
  if( skill == do_mppurge )
    return "do_mppurge";
  if( skill == do_mpscatter )
    return "do_mpscatter";
  if( skill == do_mpsound )
    return "do_mpsound";
  if( skill == do_mpsoundaround )
    return "do_mpsoundaround";
  if( skill == do_mpsoundat )
    return "do_mpsoundat";
  if( skill == do_mpstat )
    return "do_mpstat";
  if( skill == do_mptransfer )
    return "do_mptransfer";
  if( skill == do_mpunmorph )
    return "do_mpunmorph";
  if( skill == do_mpunnuisance )
    return "do_mpunnuisance";
  if( skill == do_mrange )
    return "do_mrange";
  if( skill == do_mset )
    return "do_mset";
  if( skill == do_mstat )
    return "do_mstat";
  if( skill == do_multi_disk )
    return "do_multi_disk";
  if( skill == do_multi_form )
    return "do_multi_form";
  if( skill == do_multi_eye )
    return "do_multi_eye";
  if( skill == do_murde )
    return "do_murde";
  if( skill == do_murder )
    return "do_murder";
  if( skill == do_muse )
    return "do_muse";
  if( skill == do_music )
    return "do_music";
  if( skill == do_mwhere )
    return "do_mwhere";
  if( skill == do_headbutt )
    return "do_headbutt";
  if( skill == do_roundhouse )
    return "do_roundhouse";
  if( skill == do_name )
    return "do_name";
  if( skill == do_namekfuse )
    return "do_namekfuse";
  if( skill == do_newbiechat )
    return "do_newbiechat";
  if( skill == do_newbieset )
    return "do_newbieset";
  if( skill == do_news )
    return "do_news";
#ifdef NEWSCORE
  if( skill == do_newscore )
    return "do_newscore";
#endif
  if( skill == do_newzones )
    return "do_newzones";
  if( skill == do_noemote )
    return "do_noemote";
  if( skill == do_noscream )
    return "do_noscream";
  if( skill == do_nognote )
    return "do_nognote";
  if( skill == do_nohelps )
    return "do_nohelps";
  if( skill == do_noresolve )
    return "do_noresolve";
  if( skill == do_north )
    return "do_north";
  if( skill == do_northeast )
    return "do_northeast";
  if( skill == do_northwest )
    return "do_northwest";
  if( skill == do_notell )
    return "do_notell";
  if( skill == do_noteroom )
    return "do_noteroom";
  if( skill == do_notitle )
    return "do_notitle";
  if( skill == do_nuisance )
    return "do_nuisance";
  if( skill == do_oassign )
    return "do_oassign";
  if( skill == do_ocreate )
    return "do_ocreate";
  if( skill == do_odelete )
    return "do_odelete";
  if( skill == do_ofind )
    return "do_ofind";
  if( skill == do_ofindtype )
    return "do_ofindtype";
  if( skill == do_ogrub )
    return "do_ogrub";
  if( skill == do_oinvoke )
    return "do_oinvoke";
  if( skill == do_oldscore )
    return "do_oldscore";
  if( skill == do_olist )
    return "do_olist";
  if( skill == do_omenu )
    return "do_omenu";
  if( skill == do_ooc )
    return "do_ooc";
  if( skill == do_opcopy )
    return "do_opcopy";
  if( skill == do_opedit )
    return "do_opedit";
  if( skill == do_open )
    return "do_open";
  if( skill == do_openhatch )
    return "do_openhatch";
  if( skill == do_opfind )
    return "do_opfind";
  if( skill == do_opstat )
    return "do_opstat";
  if( skill == do_orange )
    return "do_orange";
  if( skill == do_order )
    return "do_order";
  if( skill == do_orders )
    return "do_orders";
  if( skill == do_ordertalk )
    return "do_ordertalk";
  if( skill == do_oset )
    return "do_oset";
  if( skill == do_ostat )
    return "do_ostat";
  if( skill == do_outcast )
    return "do_outcast";
  if( skill == do_owhere )
    return "do_owhere";
  if( skill == do_pstatus )
    return "do_pstatus";
  if( skill == do_pcquesting )
    return "do_pcquesting";
  if( skill == do_page )
    return "do_page";
  if( skill == do_pagelen )
    return "do_pagelen";
  if( skill == do_pager )
    return "do_pager";
  if( skill == do_pardon )
    return "do_pardon";
  if( skill == do_partner )
    return "do_partner";
  if( skill == do_password )
    return "do_password";
  if( skill == do_pcrename )
    return "do_pcrename";
  if( skill == do_pclastrename )
    return "do_pclastrename";
  if( skill == do_peace )
    return "do_peace";
  if( skill == do_pfiles )
    return "do_pfiles";
  if( skill == do_pick )
    return "do_pick";
  if( skill == do_pk )
    return "do_pk";
  if( skill == do_planets )
    return "do_planets";
  if( skill == do_pldock)
    return "do_pldock";
  if( skill == do_plset )
    return "do_plset";
  if( skill == do_plist )
    return "do_plist";
  if( skill == do_pluogus )
    return "do_pluogus";
  if( skill == do_poison_weapon )
    return "do_poison_weapon";
  if( skill == do_potarafuse )
    return "do_potarafuse";
  if( skill == do_powerdown )
    return "do_powerdown";
  if( skill == do_powerup )
    return "do_powerup";
  if( skill == do_practice )
    return "do_practice";
  if( skill == do_privacy )
    return "do_privacy";
  if( skill == do_project )
    return "do_project";
  if( skill == do_promote )
    return "do_promote";
  if( skill == do_prompt )
    return "do_prompt";
  if( skill == do_pset )
    return "do_pset";
  if( skill == do_psiblast )
    return "do_psiblast";
  if( skill == do_pstat )
    return "do_pstat";
  if( skill == do_pull )
    return "do_pull";
  if( skill == do_punch )
    return "do_punch";
  if( skill == do_purge )
    return "do_purge";
  if( skill == do_push )
    return "do_push";
  if( skill == do_put )
    return "do_put";
  if( skill == do_pzap )
    return "do_pzap";
  if( skill == do_qpset )
    return "do_qpset";
  if( skill == do_qpstat )
    return "do_qpstat";
  if( skill == do_quaff )
    return "do_quaff";
  if( skill == do_games )
    return "do_games";
  if( skill == do_qui )
    return "do_qui";
  if( skill == do_quit )
    return "do_quit";
  if( skill == do_railgun )
    return "do_railgun";
  if( skill == do_revert_mystic )
    return "do_revert_mystic";
  if( skill == do_roleplay )
    return "do_roleplay";
  if( skill == do_racetalk )
    return "do_racetalk";
  if( skill == do_reqbio )
    return "do_reqbio";
  if( skill == do_resetalliances )
    return "do_resetalliances";
  if( skill == do_radar )
    return "do_radar";
  if( skill == do_rank )
    return "do_rank";
  if( skill == do_rap )
    return "do_rap";
  if( skill == do_rassign )
    return "do_rassign";
  if( skill == do_rat )
    return "do_rat";
  // if ( skill == do_rbeep                       )    return "do_rbeep";
  // if ( skill == do_rchannels                   )    return "do_rchannels";
  // if ( skill == do_rchanset                    )    return "do_rchanset";
  // if ( skill == do_rconnect                    )    return "do_rconnect";
  if( skill == do_rdelete )
    return "do_rdelete";
  // if ( skill == do_rdisconnect                 )    return "do_rdisconnect";
  if( skill == do_reboo )
    return "do_reboo";
  if( skill == do_reboot )
    return "do_reboot";
  if( skill == do_recharge )
    return "do_recharge";
  if( skill == do_recho )
    return "do_recho";
  if( skill == do_recall )
    return "do_recall";
  if( skill == do_recite )
    return "do_recite";
  if( skill == do_redit )
    return "do_redit";
  if( skill == do_redraw_page )
    return "do_redraw_page";
  if( skill == do_refresh_page )
    return "do_refresh_page";
  if( skill == do_regoto )
    return "do_regoto";
  if( skill == do_reimburse )
    return "do_reimburse";
  if( skill == do_remains )
    return "do_remains";
  if( skill == do_remove )
    return "do_remove";
  if( skill == do_rempilot )
    return "do_rempilot";
  if( skill == do_rent )
    return "do_rent";
  if( skill == do_renumber )
    return "do_renumber";
  if( skill == do_repair )
    return "do_repair";
  if( skill == do_repairset )
    return "do_repairset";
  if( skill == do_repairship )
    return "do_repairship";
  if( skill == do_repairshops )
    return "do_repairshops";
  if( skill == do_repairstat )
    return "do_repairstat";
  if( skill == do_repeat )
    return "do_repeat";
  if( skill == do_reply )
    return "do_reply";
  if( skill == do_report )
    return "do_report";
  if( skill == do_rescue )
    return "do_rescue";
  if( skill == do_reserve )
    return "do_reserve";
  if( skill == do_reset )
    return "do_reset";
  if( skill == do_resetship )
    return "do_resetship";
  if( skill == do_residence )
    return "do_residence";
  if( skill == do_rest )
    return "do_rest";
  if( skill == do_restore )
    return "do_restore";
  if( skill == do_restoretime )
    return "do_restoretime";
  if( skill == do_restrict )
    return "do_restrict";
  if( skill == do_retell )
    return "do_retell";
  if( skill == do_retire )
    return "do_retire";
  if( skill == do_retran )
    return "do_retran";
  if( skill == do_return )
    return "do_return";
  // if ( skill == do_rfinger                     )    return "do_rfinger";
  if( skill == do_rgrub )
    return "do_rgrub";
  // if ( skill == do_rignore                     )    return "do_rignore";
  // if ( skill == do_rinfo                       )    return "do_rinfo";
  if( skill == do_rings )
    return "do_rings";
  if( skill == do_rip )
    return "do_rip";
  if( skill == do_rlist )
    return "do_rlist";
  if( skill == do_rmenu )
    return "do_rmenu";
  if( skill == do_rocket_punch )
    return "do_rocket_punch";
  if( skill == do_rolldie )
    return "do_rolldie";
  if( skill == do_roster )
    return "do_roster";
  if( skill == do_rpedit )
    return "do_rpedit";
  if( skill == do_rpfind )
    return "do_rpfind";
  // if ( skill == do_rping                       )    return "do_rping";
  if( skill == do_rppedit )
    return "do_rppedit";
  if( skill == do_rpstat )
    return "do_rpstat";
  // if ( skill == do_rquery                      )    return "do_rquery";
  // if ( skill == do_rreply                      )    return "do_rreply";
  if( skill == do_rreset )
    return "do_rreset";
  if( skill == do_rset )
    return "do_rset";
  // if ( skill == do_rsockets                    )    return "do_rsockets";
  if( skill == do_rstat )
    return "do_rstat";
  // if ( skill == do_rtell                       )    return "do_rtell";
  // if ( skill == do_rwho                        )    return "do_rwho";
  // if ( skill == do_rwhois                      )    return "do_rwhois";
  if( skill == do_sacrifice )
    return "do_sacrifice";
  if( skill == do_satellite_jump )
    return "do_satellite_jump";
  if( skill == do_save )
    return "do_save";
  if( skill == do_fullsave )
    return "do_fullsave";
  if( skill == do_savealliances )
    return "do_savealliances";
  if( skill == do_solar_flare )
    return "do_solar_flare";
  if( skill == do_skga )
    return "do_skga";
  if( skill == do_savearea )
    return "do_savearea";
  if( skill == do_say )
    return "do_say";
  if( skill == do_sbc )
    return "do_sbc";
  if( skill == do_scan )
    return "do_scan";
  if( skill == do_scatter )
    return "do_scatter";
  if( skill == do_scatter_shot )
    return "do_scatter_shot";
  if( skill == do_score )
    return "do_score";
  if( skill == do_scribe )
    return "do_scribe";
  if( skill == do_search )
    return "do_search";
  if( skill == do_sedit )
    return "do_sedit";
  if( skill == do_self_destruct )
    return "do_self_destruct";
  if( skill == do_sell )
    return "do_sell";
  if( skill == do_sellhouse )
    return "do_sellhouse";
  if( skill == do_sellship )
    return "do_sellship";
  if( skill == do_sense )
    return "do_sense";
  if( skill == do_set_boot_time )
    return "do_set_boot_time";
  if( skill == do_setage )
    return "do_setage";
  if( skill == do_setclan )
    return "do_setclan";
  if( skill == do_setclass )
    return "do_setclass";
  if( skill == do_setcouncil )
    return "do_setcouncil";
  if( skill == do_setdeity )
    return "do_setdeity";
  if( skill == do_setheight )
    return "do_setheight";
  if( skill == do_setplanet )
    return "do_setplanet";
  if( skill == do_setrace )
    return "do_setrace";
  if( skill == do_setship )
    return "do_setship";
  if( skill == do_setstarsystem )
    return "do_setstarsystem";
  if( skill == do_setrestoremessage )
    return "do_setrestoremessage";
  if( skill == do_settime )
    return "do_settime";
  if( skill == do_setweather )
    return "do_setweather";
  if( skill == do_setweight )
    return "do_setweight";
  if( skill == do_ships )
    return "do_ships";
  if( skill == do_shipinfo )
    return "do_shipinfo";
  if( skill == do_shops )
    return "do_shops";
  if( skill == do_shopset )
    return "do_shopset";
  if( skill == do_shopstat )
    return "do_shopstat";
  if( skill == do_shout )
    return "do_shout";
  if( skill == do_shove )
    return "do_shove";
  if( skill == do_showrank )
    return "do_showrank";
  if( skill == do_showclan )
    return "do_showclan";
  if( skill == do_showclass )
    return "do_showclass";
  if( skill == do_showcouncil )
    return "do_showcouncil";
  if( skill == do_showdeity )
    return "do_showdeity";
  if( skill == do_showimmlog )
    return "do_showimmlog";
  if( skill == do_showlayers )
    return "do_showlayers";
  if( skill == do_showplanet )
    return "do_showplanet";
  if( skill == do_showrace )
    return "do_showrace";
  if( skill == do_showship )
    return "do_showship";
  if( skill == do_showstarsystem )
    return "do_showstarsystem";
  if( skill == do_showweather )
    return "do_showweather";
  if( skill == do_shutdow )
    return "do_shutdow";
  if( skill == do_shutdown )
    return "do_shutdown";
  if( skill == do_silence )
    return "do_silence";
  if( skill == do_sit )
    return "do_sit";
  if( skill == do_skin )
    return "do_skin";
  if( skill == do_skin_trap )
    return "do_skin_trap";
  if( skill == do_sla )
    return "do_sla";
  if( skill == do_slay )
    return "do_slay";
  if( skill == do_sleep )
    return "do_sleep";
  if( skill == do_slice )
    return "do_slice";
  if( skill == do_slist )
    return "do_slist";
  if( skill == do_slookup )
    return "do_slookup";
  if( skill == do_smoke )
    return "do_smoke";
  if( skill == do_snoop )
    return "do_snoop";
  if( skill == do_sober )
    return "do_sober";
  if( skill == do_socials )
    return "do_socials";
  if( skill == do_gocials )
    return "do_gocials";
  if( skill == do_south )
    return "do_south";
  if( skill == do_southeast )
    return "do_southeast";
  if( skill == do_southwest )
    return "do_southwest";
  if( skill == do_split_form )
    return "do_split_form";
  if( skill == do_spar )
    return "do_spar";
  if( skill == do_sparcheck )
    return "do_sparcheck";
  if( skill == do_speak )
    return "do_speak";
  if( skill == do_speeders )
    return "do_speeders";
  if( skill == do_split )
    return "do_split";
  if( skill == do_ssd_bomber )
    return "do_ssd_bomber";
  if( skill == do_sset )
    return "do_sset";
  if( skill == do_ssj )
    return "do_ssj";
  if( skill == do_ssj2 )
    return "do_ssj2";
  if( skill == do_ssj3 )
    return "do_ssj3";
    if( skill == do_ssj4 )
      return "do_ssj4";
      if( skill == do_ssjgod )
        return "do_ssjgod";
  if( skill == do_stand )
    return "do_stand";
  if( skill == do_starsystems )
    return "do_starsystems";
  if( skill == do_stat )
    return "do_stat";
  if( skill == do_statreport )
    return "do_statreport";
  if( skill == do_statshield )
    return "do_statshield";
  if( skill == do_status )
    return "do_status";
  if( skill == do_steal )
    return "do_steal";
  if( skill == do_stopspar )
    return "do_stopspar";
  if( skill == do_strew )
    return "do_strew";
  if( skill == do_strip )
    return "do_strip";
  if( skill == do_stun )
    return "do_stun";
  if( skill == do_style )
    return "do_style";
  if( skill == do_super_namek )
    return "do_super_namek";
  if( skill == do_super_nova )
    return "do_super_nova";
  if( skill == do_super_kamehameha )
    return "do_super_kamehameha";
  if( skill == do_supplicate )
    return "do_supplicate";
  if( skill == do_suppress )
    return "do_suppress";
  if( skill == do_suppress )
    return "do_suppress";
  if( skill == do_saibaman1 )
    return "do_saibaman1";
  if( skill == do_saibaman2 )
    return "do_saibaman2";
  if( skill == do_saibaman3 )
    return "do_saibaman3";
  if( skill == do_switch )
    return "do_switch";
  if( skill == do_tuffle_blow )
    return "do_tuffle_blow";
  if( skill == do_teleport_kamehameha )
    return "do_teleport_kamehameha";
  if( skill == do_t4 )
    return "do_t4";
  if( skill == do_t3 )
    return "do_t3";
  if( skill == do_t2 )
    return "do_t2";
  if( skill == do_t1 )
    return "do_t1";
  if( skill == do_vm2 )
    return "do_vm2";
  if( skill == do_vm1 )
    return "do_vm1";
  if( skill == do_vanishing_ball )
    return "do_vanishing_ball";
    if( skill == do_omega )
      return "do_omega";
      if( skill == do_overdrive_mode )
        return "do_overdrive_mode";
  if( skill == do_tag )
    return "do_tag";
  if( skill == do_tamp )
    return "do_tamp";
  if( skill == do_tribeam )
    return "do_tribeam";
  if( skill == do_tri_form )
    return "do_tri_form";
  if( skill == do_target )
    return "do_target";
  if( skill == do_tell )
    return "do_tell";
  if( skill == do_tentacle_attack )
    return "do_tentacle_attack";
  if( skill == do_think )
    return "do_think";
  if( skill == do_time )
    return "do_time";
  if( skill == do_timecmd )
    return "do_timecmd";
  if( skill == do_title )
    return "do_title";
  if( skill == do_track )
    return "do_track";
  if( skill == do_traffic )
    return "do_traffic";
  if( skill == do_train )
    return "do_train";
  if( skill == do_trajectory )
    return "do_trajectory";
  if( skill == do_transfer )
    return "do_transfer";
  if( skill == do_transform )
    return "do_transform";
//    if( skill == do_trust           ) return "do_trust";
  if( skill == do_typo )
    return "do_typo";
  if( skill == do_undelete )
    return "do_undelete";
  if( skill == do_ultra_namek )
    return "do_ultra_namek";
  if( skill == do_mechaicer )
    return "do_mechaicer";
  if( skill == do_demonic_embrace )
    return "do_demonic_embrace";
  if( skill == do_demonic_embrace )
    return "do_demonic_embrace";
  if( skill == do_angelicrapture )
    return "do_angelicrapture";
  if( skill == do_nephalimstorment )
    return "do_nephalimstorment";
  if( skill == do_unbolt )
    return "do_unbolt";
  if( skill == do_unbounty )
    return "do_unbounty";
  if( skill == do_unfoldarea )
    return "do_unfoldarea";
  if( skill == do_unhell )
    return "do_unhell";
  if( skill == do_unlock )
    return "do_unlock";
  if( skill == do_unnuisance )
    return "do_unnuisance";
  if( skill == do_unsilence )
    return "do_unsilence";
  if( skill == do_up )
    return "do_up";
  if( skill == do_uppercut )
    return "do_uppercut";
  if( skill == do_upgrade )
    return "do_upgrade";
  if( skill == do_users )
    return "do_users";
  if( skill == do_ussj )
    return "do_ussj";
  if( skill == do_ussj2 )
    return "do_ussj2";
  if( skill == do_value )
    return "do_value";
  if( skill == do_vassign )
    return "do_vassign";
  if( skill == do_version )
    return "do_version";
  if( skill == do_victories )
    return "do_victories";
  if( skill == do_viewbuf )
    return "do_viewbuf";
  if( skill == do_visible )
    return "do_visible";
  if( skill == do_vnums )
    return "do_vnums";
  if( skill == do_vsearch )
    return "do_vsearch";
  if( skill == do_wake )
    return "do_wake";
  if( skill == do_wff )
    return "do_wff";
  if( skill == do_warn )
    return "do_warn";
  if( skill == do_wartalk )
    return "do_wartalk";
  if( skill == do_watch )
    return "do_watch";
  if( skill == do_wear )
    return "do_wear";
  if( skill == do_weather )
    return "do_weather";
  if( skill == do_west )
    return "do_west";
  if( skill == do_where )
    return "do_where";
  if( skill == do_whisper )
    return "do_whisper";
  if( skill == do_who )
    return "do_who";
  if ( skill == do_whois )    
    return "do_whois";
  if( skill == do_wimpy )
    return "do_wimpy";
  if( skill == do_wizhelp )
    return "do_wizhelp";
  if( skill == do_wizinfo )
    return "do_wizinfo";
  if( skill == do_wizlist )
    return "do_wizlist";
  if( skill == do_wizlock )
    return "do_wizlock";
  if( skill == do_worth )
    return "do_worth";
  if( skill == do_wss )
    return "do_wss";
  if( skill == do_yell )
    return "do_yell";
  if( skill == do_zap )
    return "do_zap";
  if( skill == do_zones )
    return "do_zones";
  if( skill == do_dbwhere )
    return "do_dbwhere";
  if( skill == do_dbradar )
    return "do_dbradar";
  if( skill == do_dekumask )
    return "do_dekumask";
  if( skill == do_hbomb )
    return "do_hbomb";
  if( skill == do_finisher )
    return "do_finisher";
  if( skill == do_fury )
    return "do_fury";
  if( skill == do_bombarrow )
    return "do_bombarrow";
  if( skill == do_jday )
    return "do_jday";
  if( skill == do_wog )
    return "do_wog";
  if( skill == do_sharingan )
    return "do_sharingan";
  if( skill == do_cleanse )
    return "do_cleanse";
  if( skill == do_fireforge )
    return "do_fireforge";
  if( skill == do_firestorm )
    return "do_firestorm";
  if( skill == do_goronmask )
    return "do_goronmask";
  if( skill == do_firearrow )
    return "do_firearrow";
  if( skill == do_icearrow )
    return "do_icearrow";
  if( skill == do_lightarrow )
    return "do_lightarrow";
  if( skill == do_magmatouch )
    return "do_magmatouch";
  if( skill == do_novawave )
    return "do_novawave";
  if( skill == do_sflame1 )
    return "do_sflame1";
  if( skill == do_sflame2 )
    return "do_sflame2";
  if( skill == do_songofcreation )
    return "do_songofcreation";
  if( skill == do_songoftime )
    return "do_songoftime";
  if( skill == do_fiercedeity )
    return "do_fiercedeity";
  if( skill == do_uflame )
    return "do_uflame";
  if( skill == do_zoramask )
    return "do_zoramask";
  if( skill == do_shieldbash )
    return "do_shieldbash";
  if( skill == do_hoverboots )
   return "do_hoverboots";
  if( skill == do_sflame3 )
    return "do_sflame3";
  if( skill == do_hookshot )
    return "do_hookshot";
  if( skill == do_faroreswind )
    return "do_faroreswind";
  if( skill == do_puppet )
    return "do_puppet";
  if( skill == do_eruption )
    return "do_eruption";
  if( skill == do_htorch )
    return "do_htorch";
  if( skill == do_ddmb )
    return "do_ddmb";
  if( skill == do_biodrain ) // Super Skill Start
    return "do_biodrain";
  if( skill == do_bpoint )
    return "do_bpoint"; // Super Skill End
  if( skill == do_innerwill ) // Dragon Start
    return "do_innerwill";
  if( skill == do_mechaevolution )
    return "do_mechaevolution";
  if( skill == do_ancientevolution )
    return "do_ancientevolution";
  if( skill == do_dragonfire )
    return "do_dragonfire";
  if( skill == do_gasattack )
    return "do_gasattack";
  if( skill == do_dragon_thunder )
    return "do_dragon_thunder";
  if( skill == do_gust )
    return "do_gust";
  if( skill == do_buffet )
    return "do_buffet";
  if( skill == do_flare )
    return "do_flare";
  if( skill == do_atb )
    return "do_atb";
  if( skill == do_whirlwind_spin )
    return "do_whirlwind_spin"; // Dragon End
  
  if( skill == do_xbuster ) // Reploid Start
    return "do_xbuster";
  if( skill == do_pabsorb )
    return "do_pabsorb";
  if( skill == do_zsaber )
    return "do_zsaber";
  if( skill == do_lshield )
    return "do_lshield";
  if( skill == do_giga1 )
    return "do_giga1";
  if( skill == do_giga2 )
    return "do_giga2";
  if( skill == do_tslasher )
    return "do_tslasher";
  if( skill == do_ftower )
    return "do_ftower";
  if( skill == do_ghunter )
    return "do_ghunter";
  if( skill == do_aiminglaser )
    return "do_aiminglaser";
 
  if( skill == do_manifestpotara ) // SK's Manifest Potara
    return "do_manifestpotara";
 if( skill == do_copy_ability )
    return "do_copy_aility";
 if( skill == do_scan_transformation )
    return "do_scan_transformation";
 if( skill == do_reflection )
    return "do_reflection";
 if( skill == do_forget )
    return "do_forget";
 if( skill == do_changes )
    return "do_changes";
 if( skill == do_addchange )
    return "do_addchange";
 if( skill == do_addimmchange )
    return "do_addimmchange";
 if( skill == do_chedit )
    return "do_chedit";
/*//T4*/
  sprintf( buf, "(%p)", skill );
  return buf;
}

bool load_class_file( char *fname )
{
  char buf[MAX_STRING_LENGTH];
  char *word;
  bool fMatch;
  struct class_type *class;
  int cl = -1;
  int tlev = 0;
  FILE *fp;

  sprintf( buf, "%s%s", CLASS_DIR, fname );
  if( ( fp = fopen( buf, "r" ) ) == NULL )
  {
    perror( buf );
    return FALSE;
  }

  CREATE( class, struct class_type, 1 );

  /*
   * Setup defaults for additions to class structure 
   */
  class->attr_second = 0;
  class->attr_deficient = 0;
  xCLEAR_BITS( class->affected );
  class->resist = 0;
  class->suscept = 0;

  for( ;; )
  {
    word = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
      case '*':
        fMatch = TRUE;
        fread_to_eol( fp );
        break;

      case 'A':
        KEY( "Affected", class->affected, fread_bitvector( fp ) );
        KEY( "AttrPrime", class->attr_prime, fread_number( fp ) );
        KEY( "AttrSecond", class->attr_second, fread_number( fp ) );
        KEY( "AttrDeficient", class->attr_deficient, fread_number( fp ) );
        break;

      case 'C':
        KEY( "Class", cl, fread_number( fp ) );
        break;

      case 'E':
        if( !str_cmp( word, "End" ) )
        {
          fclose( fp );
          if( cl < 0 || cl >= MAX_CLASS )
          {
            sprintf( buf, "Load_class_file: Class (%s) bad/not found (%d)",
                     class->who_name ? class->who_name : "name not found", cl );
            bug( buf, 0 );
            if( class->who_name )
              STRFREE( class->who_name );
            DISPOSE( class );
            return FALSE;
          }
          class_table[cl] = class;
          return TRUE;
        }
        KEY( "ExpBase", class->exp_base, fread_number( fp ) );
        break;

      case 'G':
        KEY( "Guild", class->guild, fread_number( fp ) );
        break;

      case 'H':
        KEY( "HpMax", class->hp_max, fread_number( fp ) );
        KEY( "HpMin", class->hp_min, fread_number( fp ) );
        break;

      case 'M':
        KEY( "Mana", class->fMana, fread_number( fp ) );
        break;

      case 'N':
        KEY( "Name", class->who_name, fread_string( fp ) );
        break;

      case 'R':
        KEY( "Resist", class->resist, fread_number( fp ) );
        break;

      case 'S':
        if( !str_cmp( word, "Skill" ) )
        {
          int sn, adp;
          long double lev;

          word = fread_word( fp );
          lev = fread_number_ld( fp );
          adp = fread_number( fp );
          sn = skill_lookup( word );
          if( cl < 0 || cl >= MAX_CLASS )
          {
            sprintf( buf, "load_class_file: Skill %s -- class bad/not found (%d)", word, cl );
            bug( buf, 0 );
          }
          else if( !IS_VALID_SN( sn ) )
          {
            sprintf( buf, "load_class_file: Skill %s unknown", word );
            bug( buf, 0 );
          }
          else
          {
            skill_table[sn]->skill_level[cl] = lev;
            skill_table[sn]->skill_adept[cl] = adp;
          }
          fMatch = TRUE;
          break;
        }
        KEY( "Skilladept", class->skill_adept, fread_number( fp ) );
        KEY( "Suscept", class->suscept, fread_number( fp ) );
        break;

      case 'T':
        if( !str_cmp( word, "Title" ) )
        {
          if( cl < 0 || cl >= MAX_CLASS )
          {
            char *tmp;

            sprintf( buf, "load_class_file: Title -- class bad/not found (%d)", cl );
            bug( buf, 0 );
            tmp = fread_string_nohash( fp );
            DISPOSE( tmp );
            tmp = fread_string_nohash( fp );
            DISPOSE( tmp );
          }
          else if( tlev < MAX_LEVEL + 1 )
          {
            title_table[cl][tlev][0] = fread_string_nohash( fp );
            title_table[cl][tlev][1] = fread_string_nohash( fp );
            ++tlev;
          }
          else
            bug( "load_class_file: Too many titles" );
          fMatch = TRUE;
          break;
        }
        KEY( "Thac0", class->thac0_00, fread_number( fp ) );
        KEY( "Thac32", class->thac0_32, fread_number( fp ) );
        break;

      case 'W':
        KEY( "Weapon", class->weapon, fread_number( fp ) );
        break;
    }
    if( !fMatch )
    {
      sprintf( buf, "load_class_file: no match: %s", word );
      bug( buf, 0 );
    }
  }
  return FALSE;
}

/*
 * Load in all the class files.
 */
void load_classes(  )
{
  FILE *fpList;
  char *filename;
  char classlist[256];
  char buf[MAX_STRING_LENGTH];
  int i;

  MAX_PC_CLASS = 0;

  /*
   * Pre-init the class_table with blank classes
   */
  for( i = 0; i < MAX_CLASS; i++ )
    class_table[i] = NULL;

  sprintf( classlist, "%s%s", CLASS_DIR, CLASS_LIST );
  if( ( fpList = fopen( classlist, "r" ) ) == NULL )
  {
    perror( classlist );
    exit( 1 );
  }

  for( ;; )
  {
    filename = feof( fpList ) ? "$" : fread_word( fpList );
    if( filename[0] == '$' )
      break;

    if( !load_class_file( filename ) )
    {
      sprintf( buf, "Cannot load class file: %s", filename );
      bug( buf, 0 );
    }
    else
      MAX_PC_CLASS++;
  }
  fclose( fpList );
  for( i = 0; i < MAX_CLASS; i++ )
  {
    if( class_table[i] == NULL )
    {
      CREATE( class_table[i], struct class_type, 1 );
      create_new_class( i, "" );
    }
  }
  return;
}


void write_class_file( int cl )
{
  FILE *fpout;
  char buf[MAX_STRING_LENGTH];
  char filename[MAX_INPUT_LENGTH];
  struct class_type *class = class_table[cl];
  int x;
  long double y;

  sprintf( filename, "%s%s.class", CLASSDIR, class->who_name );
  if( ( fpout = fopen( filename, "w" ) ) == NULL )
  {
    sprintf( buf, "Cannot open: %s for writing", filename );
    bug( buf, 0 );
    return;
  }
  fprintf( fpout, "Name          %s~\n", class->who_name );
  fprintf( fpout, "Class         %d\n", cl );
  fprintf( fpout, "AttrPrime     %d\n", class->attr_prime );
  fprintf( fpout, "AttrSecond    %d\n", class->attr_second );
  fprintf( fpout, "AttrDeficient %d\n", class->attr_deficient );
  fprintf( fpout, "Weapon        %d\n", class->weapon );
  fprintf( fpout, "Guild         %d\n", class->guild );
  fprintf( fpout, "Skilladept    %d\n", class->skill_adept );
  fprintf( fpout, "Thac0         %d\n", class->thac0_00 );
  fprintf( fpout, "Thac32        %d\n", class->thac0_32 );
  fprintf( fpout, "Hpmin         %d\n", class->hp_min );
  fprintf( fpout, "Hpmax         %d\n", class->hp_max );
  fprintf( fpout, "Mana          %d\n", class->fMana );
  fprintf( fpout, "Expbase       %d\n", class->exp_base );
  fprintf( fpout, "Affected      %s\n", print_bitvector( &class->affected ) );
  fprintf( fpout, "Resist        %d\n", class->resist );
  fprintf( fpout, "Suscept       %d\n", class->suscept );
  for( x = 0; x < top_sn; x++ )
  {
    if( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
      break;
    if( ( y = skill_table[x]->skill_level[cl] ) > 0 && skill_table[x]->min_level[cl] != 0 )
      fprintf( fpout, "Skill '%s' %0.Lf %d\n", skill_table[x]->name, y, skill_table[x]->skill_adept[cl] );
  }
  for( x = 0; x <= MAX_LEVEL; x++ )
    fprintf( fpout, "Title\n%s~\n%s~\n", title_table[cl][x][0], title_table[cl][x][1] );
  fprintf( fpout, "End\n" );
  fclose( fpout );
}


/*
 * Load in all the race files.
 */
void load_races(  )
{
  FILE *fpList;
  char *filename;
  char racelist[256];
  char buf[MAX_STRING_LENGTH];
  int i;

  MAX_PC_RACE = 0;
  /*
   * Pre-init the race_table with blank races
   */
  for( i = 0; i < MAX_RACE; i++ )
    race_table[i] = NULL;

  sprintf( racelist, "%s%s", RACEDIR, RACE_LIST );
  if( ( fpList = fopen( racelist, "r" ) ) == NULL )
  {
    perror( racelist );
    exit( 1 );
  }

  for( ;; )
  {
    filename = feof( fpList ) ? "$" : fread_word( fpList );
    if( filename[0] == '$' )
      break;

    if( !load_race_file( filename ) )
    {
      sprintf( buf, "Cannot load race file: %s", filename );
      bug( buf, 0 );
    }
    else
      MAX_PC_RACE++;
  }
  for( i = 0; i < MAX_RACE; i++ )
  {
    if( race_table[i] == NULL )
    {
      CREATE( race_table[i], struct race_type, 1 );
      sprintf( race_table[i]->race_name, "%s", "unused" );
    }
  }
  fclose( fpList );
  return;
}

void write_race_file( int ra )
{
  FILE *fpout;
  char buf[MAX_STRING_LENGTH];
  char filename[MAX_INPUT_LENGTH];
  struct race_type *race = race_table[ra];
  int i;
  int x, y;

  if( !race->race_name )
  {
    sprintf( buf, "Race %d has null name, not writing .race file.", ra );
    bug( buf, 0 );
    return;
  }

  sprintf( filename, "%s%s.race", RACEDIR, race->race_name );
  if( ( fpout = fopen( filename, "w+" ) ) == NULL )
  {
    sprintf( buf, "Cannot open: %s for writing", filename );
    bug( buf, 0 );
    return;
  }

  fprintf( fpout, "Name        %s~\n", race->race_name );
  fprintf( fpout, "Race        %d\n", ra );
  fprintf( fpout, "Classes     %d\n", race->class_restriction );
  fprintf( fpout, "Str_Plus    %d\n", race->str_plus );
  fprintf( fpout, "Dex_Plus    %d\n", race->dex_plus );
  fprintf( fpout, "Int_Plus    %d\n", race->int_plus );
  fprintf( fpout, "Con_Plus    %d\n", race->con_plus );
  fprintf( fpout, "Lck_Plus    %d\n", race->lck_plus );
  fprintf( fpout, "Hit         %d\n", race->hit );
  fprintf( fpout, "Mana        %d\n", race->mana );
  fprintf( fpout, "Affected    %s\n", print_bitvector( &race->affected ) );
  fprintf( fpout, "Resist      %d\n", race->resist );
  fprintf( fpout, "Suscept     %d\n", race->suscept );
  fprintf( fpout, "Language    %d\n", race->language );
  fprintf( fpout, "Align       %d\n", race->alignment );
  fprintf( fpout, "Min_Align   %d\n", race->minalign );
  fprintf( fpout, "Max_Align	 %d\n", race->maxalign );
  fprintf( fpout, "AC_Plus     %d\n", race->ac_plus );
  fprintf( fpout, "Exp_Mult    %d\n", race->exp_multiplier );
  fprintf( fpout, "Attacks     %s\n", print_bitvector( &race->attacks ) );
  fprintf( fpout, "Defenses    %s\n", print_bitvector( &race->defenses ) );
  fprintf( fpout, "Height      %d\n", race->height );
  fprintf( fpout, "Weight      %d\n", race->weight );
  fprintf( fpout, "Hunger_Mod  %d\n", race->hunger_mod );
  fprintf( fpout, "Thirst_mod  %d\n", race->thirst_mod );
  fprintf( fpout, "Mana_Regen  %d\n", race->mana_regen );
  fprintf( fpout, "HP_Regen    %d\n", race->hp_regen );
  fprintf( fpout, "Race_Recall %d\n", race->race_recall );
  fprintf( fpout, "MaxArmor    %d\n", race->max_armor );

  for( i = 0; i < MAX_WHERE_NAME - 1; i++ )
    fprintf( fpout, "WhereName   %s~\n", race->where_name[i] );


  for( x = 0; x < top_sn; x++ )
  {
    if( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
      break;
    if( ( y = skill_table[x]->race_level[ra] ) > 0 )
      fprintf( fpout, "Skill '%s' %d %d\n", skill_table[x]->name, y, skill_table[x]->race_adept[ra] );
  }
  fprintf( fpout, "End\n" );
  fclose( fpout );
}

bool load_race_file( char *fname )
{
  char buf[MAX_STRING_LENGTH];
  char *word;
  char *race_name = NULL;
  bool fMatch;
  struct race_type *race;
  int ra = -1;
  FILE *fp;
  int i, wear = 0;

  sprintf( buf, "%s%s", RACEDIR, fname );
  if( ( fp = fopen( buf, "r" ) ) == NULL )
  {
    perror( buf );
    return FALSE;
  }

  CREATE( race, struct race_type, 1 );
  for( i = 0; i < MAX_WHERE_NAME; i++ )
    race->where_name[i] = where_name[i];
  race->max_armor = 10000;
  for( ;; )
  {
    word = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
      case '*':
        fMatch = TRUE;
        fread_to_eol( fp );
        break;

      case 'A':
        KEY( "Align", race->alignment, fread_number( fp ) );
        KEY( "AC_Plus", race->ac_plus, fread_number( fp ) );
        KEY( "Affected", race->affected, fread_bitvector( fp ) );
        KEY( "Attacks", race->attacks, fread_bitvector( fp ) );
        break;

      case 'C':
        KEY( "Con_Plus", race->con_plus, fread_number( fp ) );
        KEY( "Classes", race->class_restriction, fread_number( fp ) );
        break;


      case 'D':
        KEY( "Dex_Plus", race->dex_plus, fread_number( fp ) );
        KEY( "Defenses", race->defenses, fread_bitvector( fp ) );
        break;

      case 'E':
        if( !str_cmp( word, "End" ) )
        {
          fclose( fp );
          if( ra < 0 || ra >= MAX_RACE )
          {
            sprintf( buf, "Load_race_file: Race (%s) bad/not found (%d)",
                     race->race_name ? race->race_name : "name not found", ra );
            bug( buf, 0 );
            DISPOSE( race );
            return FALSE;
          }
          race_table[ra] = race;
          return TRUE;
        }

        KEY( "Exp_Mult", race->exp_multiplier, fread_number( fp ) );

        break;


      case 'I':
        KEY( "Int_Plus", race->int_plus, fread_number( fp ) );
        break;

      case 'H':
        KEY( "Height", race->height, fread_number( fp ) );
        KEY( "Hit", race->hit, fread_number( fp ) );
        KEY( "HP_Regen", race->hp_regen, fread_number( fp ) );
        KEY( "Hunger_Mod", race->hunger_mod, fread_number( fp ) );
        break;

      case 'L':
        KEY( "Language", race->language, fread_number( fp ) );
        KEY( "Lck_Plus", race->lck_plus, fread_number( fp ) );
        break;


      case 'M':
        KEY( "Mana", race->mana, fread_number( fp ) );
        KEY( "Mana_Regen", race->mana_regen, fread_number( fp ) );
        KEY( "Min_Align", race->minalign, fread_number( fp ) );
        race->minalign = -1000;
        KEY( "Max_Align", race->maxalign, fread_number( fp ) );
        race->maxalign = -1000;
        KEY( "MaxArmor", race->max_armor, fread_number( fp ) );
        break;

      case 'N':
        KEY( "Name", race_name, fread_string( fp ) );
        break;

      case 'R':
        KEY( "Race", ra, fread_number( fp ) );
        KEY( "Race_Recall", race->race_recall, fread_number( fp ) );
        KEY( "Resist", race->resist, fread_number( fp ) );
        break;

      case 'S':
        KEY( "Str_Plus", race->str_plus, fread_number( fp ) );
        KEY( "Suscept", race->suscept, fread_number( fp ) );
        if( !str_cmp( word, "Skill" ) )
        {
          int sn, lev, adp;

          word = fread_word( fp );
          lev = fread_number( fp );
          adp = fread_number( fp );
          sn = skill_lookup( word );
          if( ra < 0 || ra >= MAX_RACE )
          {
            sprintf( buf, "load_race_file: Skill %s -- race bad/not found (%d)", word, ra );
            bug( buf, 0 );
          }
          else if( !IS_VALID_SN( sn ) )
          {
            sprintf( buf, "load_race_file: Skill %s unknown", word );
            bug( buf, 0 );
          }
          else
          {
            skill_table[sn]->race_level[ra] = lev;
            skill_table[sn]->race_adept[ra] = adp;
          }
          fMatch = TRUE;
          break;
        }
        break;

      case 'T':
        KEY( "Thirst_Mod", race->thirst_mod, fread_number( fp ) );
        break;

      case 'W':
        KEY( "Weight", race->weight, fread_number( fp ) );
        if( !str_cmp( word, "WhereName" ) )
        {
          if( ra < 0 || ra >= MAX_RACE )
          {
            char *tmp;

            sprintf( buf, "load_race_file: Title -- race bad/not found (%d)", ra );
            bug( buf, 0 );
            tmp = fread_string_nohash( fp );
            DISPOSE( tmp );
            tmp = fread_string_nohash( fp );
            DISPOSE( tmp );
          }
          else if( wear < MAX_WHERE_NAME )
          {
            race->where_name[wear] = fread_string_nohash( fp );
            ++wear;
          }
          else
            bug( "load_race_file: Too many where_names" );
          fMatch = TRUE;
          break;
        }
        break;
    }

    if( race_name != NULL )
      sprintf( race->race_name, "%-.16s", race_name );

    if( !fMatch )
    {
      sprintf( buf, "load_race_file: no match: %s", word );
      bug( buf, 0 );
    }
  }
  return FALSE;
}

/*
 * Function used by qsort to sort skills
 */
int skill_comp( SKILLTYPE ** sk1, SKILLTYPE ** sk2 )
{
  SKILLTYPE *skill1 = ( *sk1 );
  SKILLTYPE *skill2 = ( *sk2 );

  if( !skill1 && skill2 )
    return 1;
  if( skill1 && !skill2 )
    return -1;
  if( !skill1 && !skill2 )
    return 0;
  if( skill1->type < skill2->type )
    return -1;
  if( skill1->type > skill2->type )
    return 1;
  return strcmp( skill1->name, skill2->name );
}

/*
 * Sort the skill table with qsort
 */
void sort_skill_table(  )
{
  log_string( "Sorting skill table..." );
  qsort( &skill_table[1], top_sn - 1, sizeof( SKILLTYPE * ), ( int ( * )( const void *, const void * ) )skill_comp );
}


/*
 * Remap slot numbers to sn values
 */
void remap_slot_numbers(  )
{
  SKILLTYPE *skill;
  SMAUG_AFF *aff;
  char tmp[32];
  int sn;

  log_string( "Remapping slots to sns" );

  for( sn = 0; sn <= top_sn; sn++ )
  {
    if( ( skill = skill_table[sn] ) != NULL )
    {
      for( aff = skill->affects; aff; aff = aff->next )
        if( aff->location == APPLY_WEAPONSPELL
            || aff->location == APPLY_WEARSPELL
            || aff->location == APPLY_REMOVESPELL
            || aff->location == APPLY_STRIPSN || aff->location == APPLY_RECURRINGSPELL )
        {
          sprintf( tmp, "%d", slot_lookup( atoi( aff->modifier ) ) );
          DISPOSE( aff->modifier );
          aff->modifier = str_dup( tmp );
        }
    }
  }
}

/*
 * Write skill data to a file
 */
void fwrite_skill( FILE * fpout, SKILLTYPE * skill )
{
  SMAUG_AFF *aff;
  int modifier;

  fprintf( fpout, "Name         %s~\n", skill->name );
  fprintf( fpout, "Type         %s\n", skill_tname[skill->type] );
  fprintf( fpout, "Info         %d\n", skill->info );
  fprintf( fpout, "Flags        %d\n", skill->flags );
  if( skill->target )
    fprintf( fpout, "Target       %d\n", skill->target );
  /*
   * store as new minpos (minpos>=100 flags new style in character loading)
   */
  if( skill->minimum_position )
    fprintf( fpout, "Minpos       %d\n", skill->minimum_position + 100 );
  if( skill->spell_sector )
    fprintf( fpout, "Ssector      %d\n", skill->spell_sector );
  if( skill->saves )
    fprintf( fpout, "Saves        %d\n", skill->saves );
  if( skill->slot )
    fprintf( fpout, "Slot         %d\n", skill->slot );
  if( skill->min_mana )
    fprintf( fpout, "Mana         %d\n", skill->min_mana );
  if( skill->focus )
    fprintf( fpout, "Focus        %d\n", skill->focus );
  fprintf( fpout, "CanCharge    %d\n", skill->canCharge );
  if( skill->beats )
    fprintf( fpout, "Rounds       %d\n", skill->beats );
  if( skill->range )
    fprintf( fpout, "Range        %d\n", skill->range );
  if( skill->guild != -1 )
    fprintf( fpout, "Guild        %d\n", skill->guild );
  if( skill->skill_fun )
    fprintf( fpout, "Code         %s\n", skill_name( skill->skill_fun ) );
  else if( skill->spell_fun )
    fprintf( fpout, "Code         %s\n", spell_name( skill->spell_fun ) );
  fprintf( fpout, "Dammsg       %s~\n", skill->noun_damage );
  if( skill->msg_off && skill->msg_off[0] != '\0' )
    fprintf( fpout, "Wearoff      %s~\n", skill->msg_off );

  if( skill->hit_char && skill->hit_char[0] != '\0' )
    fprintf( fpout, "Hitchar      %s~\n", skill->hit_char );
  if( skill->hit_vict && skill->hit_vict[0] != '\0' )
    fprintf( fpout, "Hitvict      %s~\n", skill->hit_vict );
  if( skill->hit_room && skill->hit_room[0] != '\0' )
    fprintf( fpout, "Hitroom      %s~\n", skill->hit_room );
  if( skill->hit_dest && skill->hit_dest[0] != '\0' )
    fprintf( fpout, "Hitdest      %s~\n", skill->hit_dest );

  if( skill->miss_char && skill->miss_char[0] != '\0' )
    fprintf( fpout, "Misschar     %s~\n", skill->miss_char );
  if( skill->miss_vict && skill->miss_vict[0] != '\0' )
    fprintf( fpout, "Missvict     %s~\n", skill->miss_vict );
  if( skill->miss_room && skill->miss_room[0] != '\0' )
    fprintf( fpout, "Missroom     %s~\n", skill->miss_room );

  if( skill->die_char && skill->die_char[0] != '\0' )
    fprintf( fpout, "Diechar      %s~\n", skill->die_char );
  if( skill->die_vict && skill->die_vict[0] != '\0' )
    fprintf( fpout, "Dievict      %s~\n", skill->die_vict );
  if( skill->die_room && skill->die_room[0] != '\0' )
    fprintf( fpout, "Dieroom      %s~\n", skill->die_room );

  if( skill->imm_char && skill->imm_char[0] != '\0' )
    fprintf( fpout, "Immchar      %s~\n", skill->imm_char );
  if( skill->imm_vict && skill->imm_vict[0] != '\0' )
    fprintf( fpout, "Immvict      %s~\n", skill->imm_vict );
  if( skill->imm_room && skill->imm_room[0] != '\0' )
    fprintf( fpout, "Immroom      %s~\n", skill->imm_room );

  if( skill->dice && skill->dice[0] != '\0' )
    fprintf( fpout, "Dice         %s~\n", skill->dice );
  if( skill->value )
    fprintf( fpout, "Value        %d\n", skill->value );
  if( skill->difficulty )
    fprintf( fpout, "Difficulty   %d\n", skill->difficulty );
  if( skill->participants )
    fprintf( fpout, "Participants %d\n", skill->participants );
  if( skill->components && skill->components[0] != '\0' )
    fprintf( fpout, "Components   %s~\n", skill->components );
  if( skill->teachers && skill->teachers[0] != '\0' )
    fprintf( fpout, "Teachers     %s~\n", skill->teachers );
  for( aff = skill->affects; aff; aff = aff->next )
  {
    fprintf( fpout, "Affect       '%s' %d ", aff->duration, aff->location );
    modifier = atoi( aff->modifier );
    if( ( aff->location == APPLY_WEAPONSPELL
          || aff->location == APPLY_WEARSPELL
          || aff->location == APPLY_REMOVESPELL
          || aff->location == APPLY_STRIPSN || aff->location == APPLY_RECURRINGSPELL ) && IS_VALID_SN( modifier ) )
      fprintf( fpout, "'%d' ", skill_table[modifier]->slot );
    else
      fprintf( fpout, "'%s' ", aff->modifier );
    fprintf( fpout, "%d\n", aff->bitvector );
  }

  if( skill->type != SKILL_HERB )
  {
    int y;
    long double min = 1000;
    for( y = 0; y < MAX_CLASS; y++ )
      if( skill->skill_level[y] < min )
        min = skill->skill_level[y];

    fprintf( fpout, "Minlevel     %0.Lf\n", min );

    min = 1000;
    for( y = 0; y < MAX_RACE; y++ )
      if( skill->race_level[y] < min )
        min = skill->race_level[y];

  }
  fprintf( fpout, "End\n\n" );
}


/*
 * Save the skill table to disk
 */
void save_skill_table(  )
{
  int x;
  FILE *fpout;

  if( ( fpout = fopen( SKILL_FILE, "w" ) ) == NULL )
  {
    perror( SKILL_FILE );
    bug( "Cannot open skills.dat for writting", 0 );
    return;
  }

  for( x = 0; x < top_sn; x++ )
  {
    if( !skill_table[x]->name || skill_table[x]->name[0] == '\0' )
      break;
    fprintf( fpout, "#SKILL\n" );
    fwrite_skill( fpout, skill_table[x] );
  }
  fprintf( fpout, "#END\n" );
  fclose( fpout );
}

/*
 * Save the herb table to disk
 */
void save_herb_table(  )
{
  int x;
  FILE *fpout;

  if( ( fpout = fopen( HERB_FILE, "w" ) ) == NULL )
  {
    bug( "Cannot open herbs.dat for writting", 0 );
    perror( HERB_FILE );
    return;
  }

  for( x = 0; x < top_herb; x++ )
  {
    if( !herb_table[x]->name || herb_table[x]->name[0] == '\0' )
      break;
    fprintf( fpout, "#HERB\n" );
    fwrite_skill( fpout, herb_table[x] );
  }
  fprintf( fpout, "#END\n" );
  fclose( fpout );
}

/*
 * Save the socials to disk
 */
void save_socials(  )
{
  FILE *fpout;
  SOCIALTYPE *social;
  int x;

  if( ( fpout = fopen( SOCIAL_FILE, "w" ) ) == NULL )
  {
    bug( "Cannot open socials.dat for writting", 0 );
    perror( SOCIAL_FILE );
    return;
  }

  for( x = 0; x < 27; x++ )
  {
    for( social = social_index[x]; social; social = social->next )
    {
      if( !social->name || social->name[0] == '\0' )
      {
        bug( "Save_socials: blank social in hash bucket %d", x );
        continue;
      }
      fprintf( fpout, "#SOCIAL\n" );
      fprintf( fpout, "Name        %s~\n", social->name );
      if( social->char_no_arg )
        fprintf( fpout, "CharNoArg   %s~\n", social->char_no_arg );
      else
        bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
      if( social->others_no_arg )
        fprintf( fpout, "OthersNoArg %s~\n", social->others_no_arg );
      if( social->char_found )
        fprintf( fpout, "CharFound   %s~\n", social->char_found );
      if( social->others_found )
        fprintf( fpout, "OthersFound %s~\n", social->others_found );
      if( social->vict_found )
        fprintf( fpout, "VictFound   %s~\n", social->vict_found );
      if( social->char_auto )
        fprintf( fpout, "CharAuto    %s~\n", social->char_auto );
      if( social->others_auto )
        fprintf( fpout, "OthersAuto  %s~\n", social->others_auto );
      fprintf( fpout, "End\n\n" );
    }
  }
  fprintf( fpout, "#END\n" );
  fclose( fpout );
}

int get_skill( char *skilltype )
{
  if( !str_cmp( skilltype, "Race" ) )
    return SKILL_RACIAL;
  if( !str_cmp( skilltype, "Spell" ) )
    return SKILL_SPELL;
  if( !str_cmp( skilltype, "Skill" ) )
    return SKILL_SKILL;
  if( !str_cmp( skilltype, "Ability" ) )
    return SKILL_ABILITY;
  if( !str_cmp( skilltype, "Weapon" ) )
    return SKILL_WEAPON;
  if( !str_cmp( skilltype, "Tongue" ) )
    return SKILL_TONGUE;
  if( !str_cmp( skilltype, "Herb" ) )
    return SKILL_HERB;
  return SKILL_UNKNOWN;
}

/*
 * Save the commands to disk
 * Added flags Aug 25, 1997 --Shaddai
 */
void save_commands(  )
{
  FILE *fpout;
  CMDTYPE *command;
  int x;

  if( ( fpout = fopen( COMMAND_FILE, "w" ) ) == NULL )
  {
    bug( "Cannot open commands.dat for writing", 0 );
    perror( COMMAND_FILE );
    return;
  }

  for( x = 0; x < 126; x++ )
  {
    for( command = command_hash[x]; command; command = command->next )
    {
      if( !command->name || command->name[0] == '\0' )
      {
        bug( "Save_commands: blank command in hash bucket %d", x );
        continue;
      }
      fprintf( fpout, "#COMMAND\n" );
      fprintf( fpout, "Name        %s~\n", command->name );
      fprintf( fpout, "Code        %s\n", skill_name( command->do_fun ) );
/* Oops I think this may be a bad thing so I changed it -- Shaddai */
      if( command->position < 100 )
        fprintf( fpout, "Position    %d\n", command->position + 100 );
      else
        fprintf( fpout, "Position    %d\n", command->position );
      fprintf( fpout, "Level       %d\n", command->level );
      fprintf( fpout, "Log         %d\n", command->log );
      if( command->flags )
        fprintf( fpout, "Flags       %d\n", command->flags );
      fprintf( fpout, "End\n\n" );
    }
  }
  fprintf( fpout, "#END\n" );
  fclose( fpout );
}

SKILLTYPE *fread_skill( FILE * fp )
{
  char buf[MAX_STRING_LENGTH];
  char *word;
  bool fMatch;
  bool got_info = FALSE;
  SKILLTYPE *skill;
  int x;

  CREATE( skill, SKILLTYPE, 1 );
  skill->slot = 0;
  skill->min_mana = 0;
  skill->focus = 0;
  skill->canCharge = FALSE;
  for( x = 0; x < MAX_CLASS; x++ )
  {
    skill->min_level[x] = 66;
    skill->skill_level[x] = 0;  /* set to 66 to disable imms getting all skills */
    skill->skill_adept[x] = 95;
  }
  for( x = 0; x < MAX_RACE; x++ )
  {
    skill->race_level[x] = 0; /* set to 66 to disable imms getting all skills */
    skill->race_adept[x] = 95;
  }
  skill->guild = -1;
  skill->target = 0;
  skill->skill_fun = NULL;
  skill->spell_fun = NULL;
  skill->spell_sector = 0;


  for( ;; )
  {
    word = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
      case '*':
        fMatch = TRUE;
        fread_to_eol( fp );
        break;

      case 'A':
        if( !str_cmp( word, "Affect" ) )
        {
          SMAUG_AFF *aff;

          CREATE( aff, SMAUG_AFF, 1 );
          aff->duration = str_dup( fread_word( fp ) );
          aff->location = fread_number( fp );
          aff->modifier = str_dup( fread_word( fp ) );
          aff->bitvector = fread_number( fp );

          if( !got_info )
          {
            for( x = 0; x < 32; x++ )
            {
              if( IS_SET( aff->bitvector, 1 << x ) )
              {
                aff->bitvector = x;
                break;
              }
            }
            if( x == 32 )
              aff->bitvector = -1;
          }
          aff->next = skill->affects;
          skill->affects = aff;
          fMatch = TRUE;
          break;
        }
        break;

      case 'C':
        KEY( "CanCharge", skill->canCharge, fread_number( fp ) );
        if( !str_cmp( word, "Class" ) )
        {
          int class = fread_number( fp );

          skill->skill_level[class] = fread_number_ld( fp );
          skill->skill_adept[class] = fread_number( fp );
          fMatch = TRUE;
          break;
        }
        if( !str_cmp( word, "Code" ) )
        {
          SPELL_FUN *spellfun;
          DO_FUN *dofun;
          char *w = fread_word( fp );

          fMatch = TRUE;
          if( ( spellfun = spell_function( w ) ) != spell_notfound )
          {
            skill->spell_fun = spellfun;
            skill->skill_fun = NULL;
          }
          else if( ( dofun = skill_function( w ) ) != skill_notfound )
          {
            skill->skill_fun = dofun;
            skill->spell_fun = NULL;
          }
          else
          {
            bug( "fread_skill: unknown skill/spell %s", w );
            skill->spell_fun = spell_null;
          }
          break;
        }
        KEY( "Code", skill->spell_fun, spell_function( fread_word( fp ) ) );
        KEY( "Components", skill->components, fread_string_nohash( fp ) );
        break;

      case 'D':
        KEY( "Dammsg", skill->noun_damage, fread_string_nohash( fp ) );
        KEY( "Dice", skill->dice, fread_string_nohash( fp ) );
        KEY( "Diechar", skill->die_char, fread_string_nohash( fp ) );
        KEY( "Dieroom", skill->die_room, fread_string_nohash( fp ) );
        KEY( "Dievict", skill->die_vict, fread_string_nohash( fp ) );
        KEY( "Difficulty", skill->difficulty, fread_number( fp ) );
        break;

      case 'E':
        if( !str_cmp( word, "End" ) )
        {
          if( skill->saves != 0 && SPELL_SAVE( skill ) == SE_NONE )
          {
            bug( "fread_skill(%s):  Has saving throw (%d) with no saving effect.", skill->name, skill->saves );
            SET_SSAV( skill, SE_NEGATE );
          }
          return skill;
        }
        break;

      case 'F':
        KEY( "Focus", skill->focus, fread_number( fp ) );
        if( !str_cmp( word, "Flags" ) )
        {
          skill->flags = fread_number( fp );
          /*
           * convert to new style     -Thoric
           */
          if( !got_info )
          {
            skill->info = skill->flags & ( BV11 - 1 );
            if( IS_SET( skill->flags, OLD_SF_SAVE_NEGATES ) )
            {
              if( IS_SET( skill->flags, OLD_SF_SAVE_HALF_DAMAGE ) )
              {
                SET_SSAV( skill, SE_QUARTERDAM );
                REMOVE_BIT( skill->flags, OLD_SF_SAVE_HALF_DAMAGE );
              }
              else
                SET_SSAV( skill, SE_NEGATE );
              REMOVE_BIT( skill->flags, OLD_SF_SAVE_NEGATES );
            }
            else if( IS_SET( skill->flags, OLD_SF_SAVE_HALF_DAMAGE ) )
            {
              SET_SSAV( skill, SE_HALFDAM );
              REMOVE_BIT( skill->flags, OLD_SF_SAVE_HALF_DAMAGE );
            }
            skill->flags >>= 11;
          }
          fMatch = TRUE;
          break;
        }
        break;

      case 'G':
        KEY( "Guild", skill->guild, fread_number( fp ) );
        break;

      case 'H':
        KEY( "Hitchar", skill->hit_char, fread_string_nohash( fp ) );
        KEY( "Hitdest", skill->hit_dest, fread_string_nohash( fp ) );
        KEY( "Hitroom", skill->hit_room, fread_string_nohash( fp ) );
        KEY( "Hitvict", skill->hit_vict, fread_string_nohash( fp ) );
        break;

      case 'I':
        KEY( "Immchar", skill->imm_char, fread_string_nohash( fp ) );
        KEY( "Immroom", skill->imm_room, fread_string_nohash( fp ) );
        KEY( "Immvict", skill->imm_vict, fread_string_nohash( fp ) );
        if( !str_cmp( word, "Info" ) )
        {
          skill->info = fread_number( fp );
          got_info = TRUE;
          fMatch = TRUE;
          break;
        }
        break;

      case 'M':
        KEY( "Mana", skill->min_mana, fread_number( fp ) );
        if( !str_cmp( word, "Minlevel" ) )
        {
          fread_to_eol( fp );
          fMatch = TRUE;
          break;
        }
        /*
         * KEY( "Minpos", skill->minimum_position, fread_number( fp ) ); 
         */
        /*
         *
         */
        if( !str_cmp( word, "Minpos" ) )
        {
          fMatch = TRUE;
          skill->minimum_position = fread_number( fp );
          if( skill->minimum_position < 100 )
          {
            switch ( skill->minimum_position )
            {
              default:
              case 0:
              case 1:
              case 2:
              case 3:
              case 4:
                break;
              case 5:
                skill->minimum_position = 6;
                break;
              case 6:
                skill->minimum_position = 8;
                break;
              case 7:
                skill->minimum_position = 9;
                break;
              case 8:
                skill->minimum_position = 12;
                break;
              case 9:
                skill->minimum_position = 13;
                break;
              case 10:
                skill->minimum_position = 14;
                break;
              case 11:
                skill->minimum_position = 15;
                break;
            }
          }
          else
            skill->minimum_position -= 100;
          break;
        }

        KEY( "Misschar", skill->miss_char, fread_string_nohash( fp ) );
        KEY( "Missroom", skill->miss_room, fread_string_nohash( fp ) );
        KEY( "Missvict", skill->miss_vict, fread_string_nohash( fp ) );
        break;

      case 'N':
        KEY( "Name", skill->name, fread_string_nohash( fp ) );
        break;

      case 'P':
        KEY( "Participants", skill->participants, fread_number( fp ) );
        break;

      case 'R':
        KEY( "Range", skill->range, fread_number( fp ) );
        KEY( "Rounds", skill->beats, fread_number( fp ) );
        if( !str_cmp( word, "Race" ) )
        {
          int race = fread_number( fp );

          skill->race_level[race] = fread_number( fp );
          skill->race_adept[race] = fread_number( fp );
          fMatch = TRUE;
          break;
        }
        break;

      case 'S':
        KEY( "Saves", skill->saves, fread_number( fp ) );
        KEY( "Slot", skill->slot, fread_number( fp ) );
        KEY( "Ssector", skill->spell_sector, fread_number( fp ) );
        break;

      case 'T':
        KEY( "Target", skill->target, fread_number( fp ) );
        KEY( "Teachers", skill->teachers, fread_string_nohash( fp ) );
        KEY( "Type", skill->type, get_skill( fread_word( fp ) ) );
        break;

      case 'V':
        KEY( "Value", skill->value, fread_number( fp ) );
        break;

      case 'W':
        KEY( "Wearoff", skill->msg_off, fread_string_nohash( fp ) );
        break;
    }

    if( !fMatch )
    {
      sprintf( buf, "Fread_skill: no match: %s", word );
      bug( buf, 0 );
    }
  }
}

void load_skill_table(  )
{
  FILE *fp;

  if( ( fp = fopen( SKILL_FILE, "r" ) ) != NULL )
  {
    top_sn = 0;
    for( ;; )
    {
      char letter;
      char *word;

      letter = fread_letter( fp );
      if( letter == '*' )
      {
        fread_to_eol( fp );
        continue;
      }

      if( letter != '#' )
      {
        bug( "Load_skill_table: # not found.", 0 );
        break;
      }

      word = fread_word( fp );
      if( !str_cmp( word, "SKILL" ) )
      {
        if( top_sn >= MAX_SKILL )
        {
          bug( "load_skill_table: more skills than MAX_SKILL %d", MAX_SKILL );
          fclose( fp );
          return;
        }
        skill_table[top_sn++] = fread_skill( fp );
        continue;
      }
      else if( !str_cmp( word, "END" ) )
        break;
      else
      {
        bug( "Load_skill_table: bad section.", 0 );
        continue;
      }
    }
    fclose( fp );
  }
  else
  {
    perror( SKILL_FILE );
    bug( "Cannot open skills.dat", 0 );
    exit( 0 );
  }
}


void load_herb_table(  )
{
  FILE *fp;

  if( ( fp = fopen( HERB_FILE, "r" ) ) != NULL )
  {
    top_herb = 0;
    for( ;; )
    {
      char letter;
      char *word;

      letter = fread_letter( fp );
      if( letter == '*' )
      {
        fread_to_eol( fp );
        continue;
      }

      if( letter != '#' )
      {
        bug( "Load_herb_table: # not found.", 0 );
        break;
      }

      word = fread_word( fp );
      if( !str_cmp( word, "HERB" ) )
      {
        if( top_herb >= MAX_HERB )
        {
          bug( "load_herb_table: more herbs than MAX_HERB %d", MAX_HERB );
          fclose( fp );
          return;
        }
        herb_table[top_herb++] = fread_skill( fp );
        if( herb_table[top_herb - 1]->slot == 0 )
          herb_table[top_herb - 1]->slot = top_herb - 1;
        continue;
      }
      else if( !str_cmp( word, "END" ) )
        break;
      else
      {
        bug( "Load_herb_table: bad section.", 0 );
        continue;
      }
    }
    fclose( fp );
  }
  else
  {
    bug( "Cannot open herbs.dat", 0 );
    exit( 0 );
  }
}

void fread_social( FILE * fp )
{
  char buf[MAX_STRING_LENGTH];
  char *word;
  bool fMatch;
  SOCIALTYPE *social;

  CREATE( social, SOCIALTYPE, 1 );

  for( ;; )
  {
    word = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
      case '*':
        fMatch = TRUE;
        fread_to_eol( fp );
        break;

      case 'C':
        KEY( "CharNoArg", social->char_no_arg, fread_string_nohash( fp ) );
        KEY( "CharFound", social->char_found, fread_string_nohash( fp ) );
        KEY( "CharAuto", social->char_auto, fread_string_nohash( fp ) );
        break;

      case 'E':
        if( !str_cmp( word, "End" ) )
        {
          if( !social->name )
          {
            bug( "Fread_social: Name not found", 0 );
            free_social( social );
            return;
          }
          if( !social->char_no_arg )
          {
            bug( "Fread_social: CharNoArg not found", 0 );
            free_social( social );
            return;
          }
          add_social( social );
          return;
        }
        break;

      case 'N':
        KEY( "Name", social->name, fread_string_nohash( fp ) );
        break;

      case 'O':
        KEY( "OthersNoArg", social->others_no_arg, fread_string_nohash( fp ) );
        KEY( "OthersFound", social->others_found, fread_string_nohash( fp ) );
        KEY( "OthersAuto", social->others_auto, fread_string_nohash( fp ) );
        break;

      case 'V':
        KEY( "VictFound", social->vict_found, fread_string_nohash( fp ) );
        break;
    }

    if( !fMatch )
    {
      sprintf( buf, "Fread_social: no match: %s", word );
      bug( buf, 0 );
    }
  }
}

void load_socials(  )
{
  FILE *fp;

  if( ( fp = fopen( SOCIAL_FILE, "r" ) ) != NULL )
  {
    top_sn = 0;
    for( ;; )
    {
      char letter;
      char *word;

      letter = fread_letter( fp );
      if( letter == '*' )
      {
        fread_to_eol( fp );
        continue;
      }

      if( letter != '#' )
      {
        bug( "Load_socials: # not found.", 0 );
        break;
      }

      word = fread_word( fp );
      if( !str_cmp( word, "SOCIAL" ) )
      {
        fread_social( fp );
        continue;
      }
      else if( !str_cmp( word, "END" ) )
        break;
      else
      {
        bug( "Load_socials: bad section.", 0 );
        continue;
      }
    }
    fclose( fp );
  }
  else
  {
    bug( "Cannot open socials.dat", 0 );
    exit( 0 );
  }
}

/*
 *  Added the flags Aug 25, 1997 --Shaddai
 */

void fread_command( FILE * fp )
{
  char buf[MAX_STRING_LENGTH];
  char *word;
  bool fMatch;
  CMDTYPE *command;

  CREATE( command, CMDTYPE, 1 );
  command->lag_count = 0; /* can't have caused lag yet... FB */
  command->flags = 0; /* Default to no flags set */

  for( ;; )
  {
    word = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
      case '*':
        fMatch = TRUE;
        fread_to_eol( fp );
        break;

      case 'C':
        KEY( "Code", command->do_fun, skill_function( fread_word( fp ) ) );
        break;

      case 'E':
        if( !str_cmp( word, "End" ) )
        {
          if( !command->name )
          {
            bug( "Fread_command: Name not found", 0 );
            free_command( command );
            return;
          }
          if( !command->do_fun )
          {
            bug( "Fread_command: Function not found", 0 );
            free_command( command );
            return;
          }
          add_command( command );
          return;
        }
        break;

      case 'F':
        KEY( "Flags", command->flags, fread_number( fp ) );
        break;

      case 'L':
        KEY( "Level", command->level, fread_number( fp ) );
        KEY( "Log", command->log, fread_number( fp ) );
        break;

      case 'N':
        KEY( "Name", command->name, fread_string_nohash( fp ) );
        break;

      case 'P':
        /*
         * KEY( "Position", command->position,  fread_number(fp) ); 
         */
        if( !str_cmp( word, "Position" ) )
        {
          fMatch = TRUE;
          command->position = fread_number( fp );
          if( command->position < 100 )
          {
            switch ( command->position )
            {
              default:
              case 0:
              case 1:
              case 2:
              case 3:
              case 4:
                break;
              case 5:
                command->position = 6;
                break;
              case 6:
                command->position = 8;
                break;
              case 7:
                command->position = 9;
                break;
              case 8:
                command->position = 12;
                break;
              case 9:
                command->position = 13;
                break;
              case 10:
                command->position = 14;
                break;
              case 11:
                command->position = 15;
                break;
            }
          }
          else
            command->position -= 100;
          break;
        }
        break;
    }

    if( !fMatch )
    {
      sprintf( buf, "Fread_command: no match: %s", word );
      bug( buf, 0 );
    }
  }
}

void load_commands(  )
{
  FILE *fp;

  if( ( fp = fopen( COMMAND_FILE, "r" ) ) != NULL )
  {
    top_sn = 0;
    for( ;; )
    {
      char letter;
      char *word;

      letter = fread_letter( fp );
      if( letter == '*' )
      {
        fread_to_eol( fp );
        continue;
      }

      if( letter != '#' )
      {
        bug( "Load_commands: # not found.", 0 );
        break;
      }

      word = fread_word( fp );
      if( !str_cmp( word, "COMMAND" ) )
      {
        fread_command( fp );
        continue;
      }
      else if( !str_cmp( word, "END" ) )
        break;
      else
      {
        bug( "Load_commands: bad section.", 0 );
        continue;
      }
    }
    fclose( fp );
  }
  else
  {
    bug( "Cannot open commands.dat", 0 );
    exit( 0 );
  }

}

void save_classes(  )
{
  int x;

  for( x = 0; x < MAX_PC_CLASS; x++ )
    write_class_file( x );
}

/*
 * Tongues / Languages loading/saving functions			-Altrag
 */
void fread_cnv( FILE * fp, LCNV_DATA ** first_cnv, LCNV_DATA ** last_cnv )
{
  LCNV_DATA *cnv;
  char letter;

  for( ;; )
  {
    letter = fread_letter( fp );
    if( letter == '~' || letter == EOF )
      break;
    ungetc( letter, fp );
    CREATE( cnv, LCNV_DATA, 1 );

    cnv->old = str_dup( fread_word( fp ) );
    cnv->olen = strlen( cnv->old );
    cnv->new = str_dup( fread_word( fp ) );
    cnv->nlen = strlen( cnv->new );
    fread_to_eol( fp );
    LINK( cnv, *first_cnv, *last_cnv, next, prev );
  }
}

void load_tongues(  )
{
  FILE *fp;
  LANG_DATA *lng;
  char *word;
  char letter;

  if( !( fp = fopen( TONGUE_FILE, "r" ) ) )
  {
    perror( "Load_tongues" );
    return;
  }
  for( ;; )
  {
    letter = fread_letter( fp );
    if( letter == EOF )
      return;
    else if( letter == '*' )
    {
      fread_to_eol( fp );
      continue;
    }
    else if( letter != '#' )
    {
      bug( "Letter '%c' not #.", letter );
      exit( 0 );
    }
    word = fread_word( fp );
    if( !str_cmp( word, "end" ) )
      return;
    fread_to_eol( fp );
    CREATE( lng, LANG_DATA, 1 );
    lng->name = STRALLOC( word );
    fread_cnv( fp, &lng->first_precnv, &lng->last_precnv );
    lng->alphabet = fread_string( fp );
    fread_cnv( fp, &lng->first_cnv, &lng->last_cnv );
    fread_to_eol( fp );
    LINK( lng, first_lang, last_lang, next, prev );
  }
  return;
}

void fwrite_langs( void )
{
  FILE *fp;
  LANG_DATA *lng;
  LCNV_DATA *cnv;

  if( !( fp = fopen( TONGUE_FILE, "w" ) ) )
  {
    perror( "fwrite_langs" );
    return;
  }
  for( lng = first_lang; lng; lng = lng->next )
  {
    fprintf( fp, "#%s\n", lng->name );
    for( cnv = lng->first_precnv; cnv; cnv = cnv->next )
      fprintf( fp, "'%s' '%s'\n", cnv->old, cnv->new );
    fprintf( fp, "~\n%s~\n", lng->alphabet );
    for( cnv = lng->first_cnv; cnv; cnv = cnv->next )
      fprintf( fp, "'%s' '%s'\n", cnv->old, cnv->new );
    fprintf( fp, "\n" );
  }
  fprintf( fp, "#end\n\n" );
  fclose( fp );
  return;
}
