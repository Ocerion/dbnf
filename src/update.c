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
 *			      Regular update module			    *
 ****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mud.h"


void mccp_interest args( ( CHAR_DATA * ch ) );

void shop_inventory_update( void );

/*
 * Local functions.
 */
float namek_regen args( ( CHAR_DATA * ch ) );
int hit_gain args( ( CHAR_DATA * ch ) );
int mana_gain args( ( CHAR_DATA * ch ) );
int move_gain args( ( CHAR_DATA * ch ) );
void mobile_update args( ( void ) );
void weather_update args( ( void ) );
void time_update args( ( void ) );  /* FB */
void char_update args( ( void ) );
void mystic_check args( ( void ) );
void dball_check args( ( void ) );
void mana_check args( ( void ) );
void summon_update args( ( void ) );
void obj_update args( ( void ) );
void aggr_update args( ( void ) );
void room_act_update args( ( void ) );
void obj_act_update args( ( void ) );
void char_check args( ( void ) );
void drunk_randoms args( ( CHAR_DATA * ch ) );
void hallucinations args( ( CHAR_DATA * ch ) );
void subtract_times args( ( struct timeval * etime, struct timeval * stime ) );
void transform_oozaru args( ( CHAR_DATA * ch ) );
void untransform_oozaru args( ( CHAR_DATA * ch ) );
void transform_golden_oozaru args( ( CHAR_DATA * ch ) );
void untransform_golden_oozaru args( ( CHAR_DATA * ch ) );
void transform_makeo args( ( CHAR_DATA * ch ) );
void untransform_makeo args( ( CHAR_DATA * ch ) );
void rank_update args( ( void ) );
void ladderTableClear args( ( int mon ) );

/* weather functions - FB */
void adjust_vectors args( ( WEATHER_DATA * weather ) );
void get_weather_echo args( ( WEATHER_DATA * weather ) );
void get_time_echo args( ( WEATHER_DATA * weather ) );
char *get_moon_echo args( ( WEATHER_DATA * weather ) );

/*
 * Global Variables
 */
CHAR_DATA *gch_prev;
OBJ_DATA *gobj_prev;

CHAR_DATA *timechar;

char *corpse_descs[] = {
  "The corpse of %s is in the last stages of decay.",
  "The corpse of %s is crawling with vermin.",
  "The corpse of %s fills the air with a foul stench.",
  "The corpse of %s is buzzing with flies.",
  "The corpse of %s lies here."
};

extern int top_exit;

void rank_update() {
  /* Defunct Function */
}

void save_economy( void )
{
  char buf[MAX_STRING_LENGTH];
  FILE *fpout;
  AREA_DATA *area;

  for( area = first_area; area; area = area->next )
  {

    sprintf( buf, "%s.econ", area->filename );
    if( ( fpout = fopen( buf, "w" ) ) == NULL )
    {
      bug( "save_economy: fopen", 0 );
      perror( buf );
      return;
    }

    fprintf( fpout, "%.0f\n", area->economy );
    fclose( fpout );
  }
  return;
}

/*
 *  The actual oozaru morph
 */
void transform_oozaru( CHAR_DATA * ch )
{

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    transStatRemove( ch );  // Bug found thanks to Tirion. -Karma
    xREMOVE_BIT( ch->affected_by, AFF_KAIOKEN );
    xREMOVE_BIT( ch->affected_by, AFF_MYSTIC );
    xREMOVE_BIT( ch->affected_by, AFF_MAJIN );
  }

  act( AT_WHITE, "You gaze up at the full moon and you feel your self changing.", ch, NULL, NULL, TO_CHAR );
  act( AT_YELLOW, "You let out an earth shattering howl as you transform into Oozaru!", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "$n gazes up at the full moon and $e begins to change.", ch, NULL, NULL, TO_NOTVICT );
  act( AT_YELLOW, "$n lets out an earth shattering howl as $e transforms into Oozaru!", ch, NULL, NULL, TO_NOTVICT );

  xSET_BIT( ch->affected_by, AFF_OOZARU );
  ch->pl = ch->exp * 10;

  return;
}

void untransform_oozaru( CHAR_DATA * ch )
{
	if( !xIS_SET( ( ch )->affected_by, AFF_FAKEMOON ) )
	{

  act( AT_WHITE, "As the full moon fades and the sun rises your body begins to lose the effects", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "the transformation has brought about. In a timely fashion, your monstrous", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "appearance begins to diminish as you bask in the glow of the dawn a new day", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "brings. The hideous form that was once present has now been replaced with a", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "normal exterior as your relentless rage calms and your power recedes.", ch, NULL, NULL, TO_CHAR );

  act( AT_WHITE, "As the full moon fades and the sun rises $n's body begins to lose the", ch, NULL, NULL, TO_NOTVICT );
  act( AT_WHITE, "effects the transformation has brought about. In a timely fashion, $s monstrous", ch, NULL, NULL,
       TO_NOTVICT );
  act( AT_WHITE, "appearance begins to diminish as $e basks in the glow of the dawn a new day", ch, NULL, NULL, TO_NOTVICT );
  act( AT_WHITE, "brings. The hideous form that was once present has now been replaced with a", ch, NULL, NULL, TO_NOTVICT );
  act( AT_WHITE, "normal exterior as $s relentless rage calms and $s power recedes.", ch, NULL, NULL, TO_NOTVICT );

  xREMOVE_BIT( ch->affected_by, AFF_OOZARU );
  ch->pl = ch->exp;

  /*
   * Added to represent when after a person dropped from Oozaru,
   * they were incredibly drained of energy. -Karma
   */
  ch->mana = 0;

  return;
	}
	else
	{
		return;
	}
}

void transform_makeo( CHAR_DATA * ch )
{

  if( xIS_SET( ch->affected_by, AFF_MAKEOSTAR ) )
    return;

  act( AT_RED, "Sinister light from the Makeo Star shines down upon you.", ch, NULL, NULL, TO_CHAR );
  act( AT_RED, "You yell out as the evil energy radiating from the Makeo Star increases your power!", ch, NULL, NULL,
       TO_CHAR );
  act( AT_RED, "Sinister light from the Makeo Star shines down upon $n.", ch, NULL, NULL, TO_NOTVICT );
  act( AT_RED, "$n yells out as the evil energy radiating from the Makeo Star increases $s power!", ch, NULL, NULL,
       TO_NOTVICT );

  xSET_BIT( ch->affected_by, AFF_MAKEOSTAR );
  xREMOVE_BIT( ch->affected_by, AFF_MAJIN );

  if( xIS_SET( ch->affected_by, AFF_EVILBOOST ) ||
      xIS_SET( ch->affected_by, AFF_EVILSURGE ) || xIS_SET( ch->affected_by, AFF_EVILOVERLOAD ) )
    ch->pl = ch->exp * ( 10 + ch->evilmod );
  else
    ch->pl = ch->exp * 10;

  return;
}

void untransform_makeo( CHAR_DATA * ch )
{
  xREMOVE_BIT( ch->affected_by, AFF_MAKEOSTAR );

  act( AT_DGREY, "The Makeo Star leaves the Earth's orbit, returning you to normal.", ch, NULL, NULL, TO_CHAR );
  act( AT_DGREY, "$n returns to normal.", ch, NULL, NULL, TO_NOTVICT );

  if( xIS_SET( ch->affected_by, AFF_EVILBOOST ) ||
      xIS_SET( ch->affected_by, AFF_EVILSURGE ) || xIS_SET( ch->affected_by, AFF_EVILOVERLOAD ) )
    ch->pl = ch->exp * ch->evilmod;
  else
    ch->pl = ch->exp;

  return;
}

/*
 *  Golden Oozaru. -Karma
 */
void transform_golden_oozaru( CHAR_DATA * ch )
{

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    transStatRemove( ch );
    xREMOVE_BIT( ch->affected_by, AFF_KAIOKEN );
    xREMOVE_BIT( ch->affected_by, AFF_MYSTIC );
    xREMOVE_BIT( ch->affected_by, AFF_MAJIN );
  }

  act( AT_WHITE, "You gaze up at the full moon and you feel your self changing.", ch, NULL, NULL, TO_CHAR );
  act( AT_YELLOW, "You let out an earth shattering howl as you transform into a Golden Oozaru!", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "$n gazes up at the full moon and $e begins to change.", ch, NULL, NULL, TO_NOTVICT );
  act( AT_YELLOW, "$n lets out an earth shattering howl as $e transforms into a Golden Oozaru!", ch, NULL, NULL,
       TO_NOTVICT );

  xSET_BIT( ch->affected_by, AFF_GOLDEN_OOZARU );
  ch->pl = ch->exp * 35;

  return;
}

void untransform_golden_oozaru( CHAR_DATA * ch )
{
		if( !xIS_SET( ( ch )->affected_by, AFF_FAKEMOON ) )
	{

  act( AT_WHITE, "The full moon fades as the sun rises and you return to normal.", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "$n returns to normal.", ch, NULL, NULL, TO_NOTVICT );

  xREMOVE_BIT( ch->affected_by, AFF_GOLDEN_OOZARU );
  ch->pl = ch->exp;

  /*
   * Added to represent when after a person dropped from Oozaru,
   * they were incredibly drained of energy. -Karma
   */
  ch->mana = 0;

  return;
	}
	else
	{
		return;
	}
}



/*
 * Advancement stuff.
 */
/************************************************************************
 * this goes where you want. should be called by probably update_char.  *
 * or weather_update, like once a day, so hair doesn't grow super fast. *
 ************************************************************************/

void hair_growth( CHAR_DATA * ch )
{
  if( IS_NPC( ch ) )
  {
    send_to_char( "This will only affect players.\n\r", ch );
    return;
  }

  ch->pcdata->hairlen += 1;

  switch ( ch->pcdata->hairstyle )
  {
    case STYLE_NONE:
      ch->pcdata->hairlen -= 1; /* As STYLE_NONE won't grow, but simular to bald in a sense */
      break;
    case STYLE_BALD:
      if( ch->pcdata->hairlen >= 3 )
      {
        ch->pcdata->hairstyle = STYLE_CREW;
        send_to_char( "Your hair has grown enough that it is now a crew-cut.\n\r", ch );
      }
      break;
    case STYLE_CREW:
      if( ch->pcdata->hairlen >= 8 )
      {
        ch->pcdata->hairstyle = STYLE_SHORT_BANGS;
        send_to_char( "Your hair has grown enough that it is now short bangs.\n\r", ch );
      }
      break;
    case STYLE_SHORT_BANGS:
      if( ch->pcdata->hairlen >= 13 )
      {
        send_to_char( "Your hair has grown enough that it is now medium length bangs instead of short bangs..\n\r", ch );
        ch->pcdata->hairstyle = STYLE_MEDIUM_BANGS;
      }
      break;
    case STYLE_MEDIUM_BANGS:
      if( ch->pcdata->hairlen >= 20 )
      {
        send_to_char( "Your hair has grown enough that it is now long length bangs instead of medium bangs..\n\r", ch );
        ch->pcdata->hairstyle = STYLE_LONG_BANGS;
      }
      break;
    case STYLE_LONG_BANGS:
    case STYLE_PONYTAIL:
    case STYLE_FLOWING:
      send_to_char( "Your hair style isn't setup yet.\n\r", ch );
      break;
    default:
      bug( "hair_growth: invalid hairstyle for %s.", ch->name, 0 );
  }
}

void advance_level( CHAR_DATA * ch )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
//    int add_hp;
  int add_mana = 0;
  int add_move;
  int add_prac = 0;

/*	save_char_obj( ch );*/
  sprintf( buf, "the %s", title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0] );
  set_title( ch, buf );

//    add_hp  = con_app[get_curr_con(ch)].hitp + number_range(
//        _table[ch->class]->hp_min,
//        class_table[ch->class]->hp_max );
/*    add_mana	= class_table[ch->class]->fMana
		    ? number_range(2, (2*get_curr_int(ch)+get_curr_wis(ch))/8)
		    : 0;
*/
  add_move = number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
//    add_prac  = wis_app[get_curr_wis(ch)].practice;

//    add_hp  = UMAX(  1, add_hp   );
  add_mana = UMAX( 0, add_mana );
  add_move = UMAX( 10, add_move );

  /*
   * bonus for deadlies 
   */
  if( IS_PKILL( ch ) )
  {
    add_mana = add_mana + add_mana * .3;
    add_move = add_move + add_move * .3;
//        add_hp +=1; /* bitch at blod if you don't like this :) */
//    sprintf( buf, "Gravoc's Pandect steels your sinews.\n\r" );
  }

  ch->max_hit = 100;
  ch->max_mana += add_mana;
  ch->max_move += add_move;
  ch->practice += add_prac;

  if( !IS_NPC( ch ) )
    xREMOVE_BIT( ch->act, PLR_BOUGHT_PET );

  if( ch->level == LEVEL_AVATAR )
  {
    DESCRIPTOR_DATA *d;

    //sprintf( buf, "%s has just achieved Avatarhood!", ch->name );
    for( d = first_descriptor; d; d = d->next )
      if( d->connected == CON_PLAYING && d->character != ch )
      {
        set_char_color( AT_IMMORT, d->character );
        //      send_to_char( buf, d->character );
        send_to_char( "\n\r", d->character );
      }
    set_char_color( AT_WHITE, ch );
    do_help( ch, "M_ADVHERO_" );
  }
  if( ch->level < LEVEL_IMMORTAL )
  {
//    sprintf( buf, "Your gain is: 0/0 hp, %d/%d mana, %d/%d move, %d/%d prac.\n\r",
//    add_hp, ch->max_hit,
//             add_mana, ch->max_mana, add_move, ch->max_move, add_prac, ch->practice );
    set_char_color( AT_WHITE, ch );
//    send_to_char( buf, ch );
    if( !IS_NPC( ch ) )
    {
      sprintf( buf2, "&G%-13s  ->&w%-2d  &G-&w  %-5d&G   Rvnum: %-5d   %s %s",
               ch->name,
               ch->level,
               get_age( ch ),
               ch->in_room == NULL ? 0 : ch->in_room->vnum,
               capitalize( race_table[ch->race]->race_name ), class_table[ch->class]->who_name );
      append_to_file( PLEVEL_FILE, buf2 );
    }
  }
  return;
}

void do_plset( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *v;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument( argument, arg );

  if( ( v = get_char_room( ch, arg ) ) == NULL )
  {
    ch_printf( ch, "Nobody here by that name.\n\r" );
    return;
  }
  gain_exp( v, atoi( argument ) );
}

void gain_exp( CHAR_DATA * ch, long double gain )
{
  char buf[MAX_STRING_LENGTH];
/*    int modgain;*/
  long double modgain;
  int energygain;
  float pl_mult = 1;
  int rank = 0, newRank = 0;
  int trueRank = 0;

  if( IS_NPC( ch ) )
    return;

  if( IS_IMMORTAL( ch ) )
    return;

  modgain = gain;

  rank = get_rank_number( ch );
  trueRank = get_true_rank( ch );
  /*
   * per-race experience multipliers 
   */
  modgain *= ( long double )( race_table[ch->race]->exp_multiplier / 100.0 );
  if( ch->exp != ch->pl )
  {
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
    {
      pl_mult = ( float )ch->heart_pl / ch->pl;
      ch->heart_pl += modgain * pl_mult;
    }
    pl_mult = ( float )ch->pl / ch->exp;
    ch->pl += modgain * pl_mult;
  }
  else
  {
    ch->pl += modgain;
    ch->heart_pl += modgain;
  }

  ch->exp += modgain;

  if( ch->exp <= 0 )
    ch->exp = 1;
  if( ch->pl <= 0 )
    ch->pl = 1;

  if( ch->pcdata->clan )
  {
    ch->pcdata->clan->clanPL += modgain;
    save_clan( ch->pcdata->clan );
  }
  update_plHiscore( ch );


  if( NOT_AUTHED( ch ) && ch->exp >= MAX_EXP_PRE_AUTH )
  {
    send_to_char( "Your power level can not increase until you are authorized.\n\r", ch );
    ch->exp = MAX_EXP_PRE_AUTH;
    ch->pl = ( ch->pl - ( ch->pl - MAX_EXP_PRE_AUTH ) );
    return;
  }

  if( rank != get_rank_number( ch ) )
  {
    sprintf( buf, "%s's rank has changed to %s", ch->name, get_rank_color( ch ) );
    do_info( ch, buf );
  }

  if( trueRank != ( newRank = get_true_rank( ch ) ) )
  {
    ch->level = newRank;

    if( ch->level == 10 && !xIS_SET( ch->act, PLR_SKAI ) )
    {
      ch->perm_str += 5;
      ch->perm_dex += 5;
      ch->perm_int += 5;
      ch->perm_con += 5;
      ch->perm_lck += 5;
      xSET_BIT( ch->act, PLR_SKAI );
      do_help( ch, "ascendant" );
    }

    if( ch->level == 11 )
    {
      do_help( ch, "transcendent" );
    }

    if( ch->level == 12 )
    {
      do_help( ch, "champion" );
    }

    if( ch->level == 13 )
    {
      do_help( ch, "titans" );
    }

    if( ch->level == 14 )
    {
      do_help( ch, "mythical" );
    }

    if( ch->level == 15 ) //&& !xSET_BIT( ch->act, PLR_WKAI ))
    {
      ch->max_hit += 100;
      do_help( ch, "omnipotent" );
      //xSET_BIT( ch->act, PLR_WKAI );
    }

    if( ch->level == 16 )
    {
      if( !IS_SET( ch->pcdata->flags, PCFLAG_IMMORTALITY ) )
        SET_BIT( ch->pcdata->flags, PCFLAG_IMMORTALITY );

      xSET_BIT( ch->act, PLR_DG );
      do_help( ch, "demigod" );
    }

  }

/*  PL high enough to gain a train point */

  if( ( ch->exp >= ( pow( ch->max_train, 3.07 ) * 10000 ) ) && ( ch->max_train < 72 ) )
  {
    set_char_color( AT_LBLUE + AT_BLINK, ch );
    if( is_android( ch ) || is_reploid( ch ) )
      ch_printf( ch, "You gained 10 additional hardware upgrade points!\n\r" );
    else
      ch_printf( ch, "You gained 10 additional training points!\n\r" );
    ch->train += 10;
    ch->max_train += 1;
  }

/*  PL high enough to gain a practice point */

/*  Disabled for DBS v2.0

    if ( ( ch->exp >= ( (ch->max_prac - 1) * (ch->max_prac - 1) * 100000) )
    	&& (ch->max_prac < 30) )
    {
	set_char_color( AT_LBLUE + AT_BLINK, ch );
	if (is_android(ch) || is_reploid( ch ) )
	ch_printf( ch, "You gained an additional software upgrade point!\n\r" );
	else
	ch_printf( ch, "You gained an additional practice point!\n\r" );

	ch->practice += 1;
	ch->max_prac += 1;
    }
*/

/*  PL high enough to gain more energy */

  if( ch->exp >= ( pow( ch->max_energy, 2.27 ) * 750 ) && ch->max_mana < 999999 )
  {
    set_char_color( AT_LBLUE + AT_BLINK, ch );
    ch_printf( ch, "You gained some more energy!\n\r" );

    energygain = ( double )( ( double )get_curr_con( ch ) / 100 + 1 ) * get_curr_con( ch ) + number_range( 5, 15 );
    ch->mana += energygain;
    ch->max_mana += energygain;
    ch->max_mana = URANGE( 100, ch->max_mana, 999999 );
    ch->mana = UMIN( ch->mana, ch->max_mana );
    ch->max_energy += 1;
  }

  return;
}

/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA * ch )
{
  int gain;
  if( IS_NPC( ch ) )
  {
    gain = number_range( 6, 8 );
  }
  else if( is_saibaman( ch ) && ( time_info.hour <= 20 || time_info.hour >= 5 ) )
  {
    gain = number_range( 9, 12 );
  }
  else
  {
    gain = number_range( 5, 7 );

    switch ( ch->position )
    {
      case POS_DEAD:
        return 0;
      case POS_MORTAL:
        gain = number_range( 1, 3 );
      case POS_INCAP:
        gain = number_range( 1, 3 );
      case POS_STUNNED:
        gain = number_range( 1, 3 );
      case POS_SLEEPING:
        gain *= 2.0;
        break;
      case POS_RESTING:
        gain *= 1.25;
        break;
    }

    if( ch->pcdata->condition[COND_FULL] == 0 )
      gain /= 2;

    if( ch->pcdata->condition[COND_THIRST] == 0 )
      gain /= 2;
  }

  if( IS_AFFECTED( ch, AFF_POISON ) )
    gain /= 4;

  if( xIS_SET( ( ch )->in_room->room_flags, ROOM_HTANKS ) )
    gain *= 2;

  if( ch->race == 3 )
    gain += get_curr_con( ch ) / 10;
  else
    gain += get_curr_con( ch ) / 15;

  return UMIN( gain, ch->max_hit - ch->hit );
}



int mana_gain( CHAR_DATA * ch )
{
  int gain;

  if( IS_NPC( ch ) )
  {
    gain = number_range( 6, 8 );
  }
  else
  {
    gain = number_range( 5, 7 );

    switch ( ch->position )
    {
      case POS_DEAD:
        return 0;
      case POS_MORTAL:
        gain = number_range( 1, 3 );
      case POS_INCAP:
        gain = number_range( 1, 3 );
      case POS_STUNNED:
        gain = number_range( 1, 3 );
      case POS_SLEEPING:
        gain *= 2.0;
        break;
      case POS_RESTING:
        gain *= 1.25;
        break;
    }

    if( ch->pcdata->condition[COND_FULL] == 0 )
      gain /= 2;

    if( ch->pcdata->condition[COND_THIRST] == 0 )
      gain /= 2;
  }

  if( IS_AFFECTED( ch, AFF_POISON ) )
    gain /= 4;

  if( xIS_SET( ( ch )->in_room->room_flags, ROOM_HTANKS ) )
    gain *= 2;

  if( ch->race == 3 )
    gain += get_curr_con( ch ) / 10;
  else
    gain += get_curr_con( ch ) / 15;

  gain = ( double )gain / 200 * ch->max_mana;

  return UMIN( gain, ch->max_mana - ch->mana );
}


/*
int move_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMAX( 15, 2 * ch->level );

	switch ( ch->position )
	{
	case POS_DEAD:	   return 0;
	case POS_MORTAL:   return -1;
	case POS_INCAP:    return -1;
	case POS_STUNNED:  return 1;
	case POS_SLEEPING: gain += get_curr_dex(ch) * 4.5;	break;
	case POS_RESTING:  gain += get_curr_dex(ch) * 2.5;	break;
	}


	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    return UMIN(gain, ch->max_move - ch->move);
}
*/

void gain_condition( CHAR_DATA * ch, int iCond, int value )
{
  int condition;
  ch_ret retcode = rNONE;

  if( value == 0 || IS_NPC( ch ) || ch->level >= LEVEL_IMMORTAL || NOT_AUTHED( ch ) )
    return;

  condition = ch->pcdata->condition[iCond];
  ch->pcdata->condition[iCond] = URANGE( 0, condition + value, 68 );

  if( ch->pcdata->condition[iCond] == 0 )
  {
    switch ( iCond )
    {
      case COND_FULL:
        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_HUNGRY, ch );
          send_to_char( "You are STARVING!\n\r", ch );
          act( AT_HUNGRY, "$n is starved half to death!", ch, NULL, NULL, TO_ROOM );
          if( !IS_PKILL( ch ) || number_bits( 1 ) == 0 )
            worsen_mental_state( ch, 1 );
          retcode = damage( ch, ch, 1, TYPE_UNDEFINED );
        }
        break;

      case COND_THIRST:
        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_THIRSTY, ch );
          send_to_char( "You are DYING of THIRST!\n\r", ch );
          act( AT_THIRSTY, "$n is dying of thirst!", ch, NULL, NULL, TO_ROOM );
          worsen_mental_state( ch, IS_PKILL( ch ) ? 1 : 2 );
          retcode = damage( ch, ch, 2, TYPE_UNDEFINED );
        }
        break;

      case COND_DRUNK:
        if( condition != 0 )
        {
          set_char_color( AT_SOBER, ch );
          send_to_char( "You are sober.\n\r", ch );
        }
        retcode = rNONE;
        break;
      default:
        bug( "Gain_condition: invalid condition type %d", iCond );
        retcode = rNONE;
        break;
    }
  }

  if( retcode != rNONE )
    return;

  if( ch->pcdata->condition[iCond] == 1 )
  {
    switch ( iCond )
    {
      case COND_FULL:
        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_HUNGRY, ch );
          send_to_char( "You are really hungry.\n\r", ch );
          act( AT_HUNGRY, "You can hear $n's stomach growling.", ch, NULL, NULL, TO_ROOM );
          if( number_bits( 1 ) == 0 )
            worsen_mental_state( ch, 1 );
        }
        break;

      case COND_THIRST:
        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_THIRSTY, ch );
          send_to_char( "You are really thirsty.\n\r", ch );
          worsen_mental_state( ch, 1 );
          act( AT_THIRSTY, "$n looks a little parched.", ch, NULL, NULL, TO_ROOM );
        }
        break;

      case COND_DRUNK:
        if( condition != 0 )
        {
          set_char_color( AT_SOBER, ch );
          send_to_char( "You are feeling a little less light headed.\n\r", ch );
        }
        break;
    }
  }


  if( ch->pcdata->condition[iCond] == 2 )
  {
    switch ( iCond )
    {
      case COND_FULL:

        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_HUNGRY, ch );
          send_to_char( "You are hungry.\n\r", ch );
        }
        break;

      case COND_THIRST:
        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_THIRSTY, ch );
          send_to_char( "You are thirsty.\n\r", ch );
        }
        break;

    }
  }

  if( ch->pcdata->condition[iCond] == 3 )
  {
    switch ( iCond )
    {
      case COND_FULL:
        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_HUNGRY, ch );
          send_to_char( "You are a mite peckish.\n\r", ch );
        }
        break;

      case COND_THIRST:
        if( ch->level < LEVEL_AVATAR )
        {
          set_char_color( AT_THIRSTY, ch );
          send_to_char( "You could use a sip of something refreshing.\n\r", ch );
        }
        break;

    }
  }


  return;
}


/*
 * Put this in a seperate function so it isn't called three times per tick
 * This was added after a suggestion from Cronel	--Shaddai
 */

void check_alignment( CHAR_DATA * ch )
{
  /*
   *  Race alignment restrictions, h
   */
  if( ch->alignment < race_table[ch->race]->minalign )
  {
    set_char_color( AT_BLOOD, ch );
    send_to_char( "Your actions have been incompatible with the ideals of your race.  This troubles you.", ch );
  }

  if( ch->alignment > race_table[ch->race]->maxalign )
  {
    set_char_color( AT_BLOOD, ch );
    send_to_char( "Your actions have been incompatible with the ideals of your race.  This troubles you.", ch );
  }

}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Mud cpu time.
 */
void mobile_update( void )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *ch;
  EXIT_DATA *pexit;
  int door;
  ch_ret retcode;

  retcode = rNONE;

  /*
   * Examine all mobs. 
   */
  for( ch = last_char; ch; ch = gch_prev )
  {
    set_cur_char( ch );
    if( ch == first_char && ch->prev )
    {
      bug( "mobile_update: first_char->prev != NULL... fixed", 0 );
      ch->prev = NULL;
    }

    gch_prev = ch->prev;

    if( gch_prev && gch_prev->next != ch )
    {
      sprintf( buf, "FATAL: Mobile_update: %s->prev->next doesn't point to ch.", ch->name );
      bug( buf, 0 );
      bug( "Short-cutting here", 0 );
      gch_prev = NULL;
      ch->prev = NULL;
      do_shout( ch, "Goku says, 'Prepare for the worst!'" );
    }

    if( !IS_NPC( ch ) )
    {
      drunk_randoms( ch );
      hallucinations( ch );
      continue;
    }

    if( !ch->in_room || IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_PARALYSIS ) )
      continue;

/* Clean up 'animated corpses' that are not charmed' - Scryn */

    if( ch->pIndexData->vnum == 5 && !IS_AFFECTED( ch, AFF_CHARM ) )
    {
      if( ch->in_room->first_person )
        act( AT_MAGIC, "$n returns to the dust from whence $e came.", ch, NULL, NULL, TO_ROOM );

      if( IS_NPC( ch ) )  /* Guard against purging switched? */
        extract_char( ch, TRUE );
      continue;
    }

    if( !xIS_SET( ch->act, ACT_RUNNING ) && !xIS_SET( ch->act, ACT_SENTINEL ) && !ch->fighting && ch->hunting )
    {
      WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
      /*
       * Commented out temporarily to avoid spam - Scryn
       * sprintf( buf, "%s hunting %s from %s.", ch->name,
       * ch->hunting->name,
       * ch->in_room->name );
       * log_string( buf ); 
       */
      hunt_victim( ch );
      continue;
    }

    /*
     * Examine call for special procedure 
     */
    if( !xIS_SET( ch->act, ACT_RUNNING ) && ch->spec_fun )
    {
      if( ( *ch->spec_fun ) ( ch ) )
        continue;
      if( char_died( ch ) )
        continue;
    }

    /*
     * Check for mudprogram script on mob 
     */
    if( HAS_PROG( ch->pIndexData, SCRIPT_PROG ) )
    {
      mprog_script_trigger( ch );
      continue;
    }

    if( ch != cur_char )
    {
      bug( "Mobile_update: ch != cur_char after spec_fun", 0 );
      continue;
    }

    /*
     * That's all for sleeping / busy monster 
     */
    if( ch->position != POS_STANDING )
      continue;

    if( xIS_SET( ch->act, ACT_MOUNTED ) )
    {
      if( xIS_SET( ch->act, ACT_AGGRESSIVE ) || xIS_SET( ch->act, ACT_META_AGGR ) )
        do_emote( ch, "snarls and growls." );
      continue;
    }

    if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE )
        && ( xIS_SET( ch->act, ACT_AGGRESSIVE ) || xIS_SET( ch->act, ACT_META_AGGR ) ) )
      do_emote( ch, "glares around and snarls." );


    /*
     * MOBprogram random trigger 
     */
    if( ch->in_room->area->nplayer > 0 )
    {
      mprog_random_trigger( ch );
      if( char_died( ch ) )
        continue;
      if( ch->position < POS_STANDING )
        continue;
    }

    /*
     * MOBprogram hour trigger: do something for an hour 
     */
    mprog_hour_trigger( ch );

    if( char_died( ch ) )
      continue;

    rprog_hour_trigger( ch );
    if( char_died( ch ) )
      continue;

    if( ch->position < POS_STANDING )
      continue;

    /*
     * Scavenge 
     */
    if( xIS_SET( ch->act, ACT_SCAVENGER ) && ch->in_room->first_content && number_bits( 2 ) == 0 )
    {
      OBJ_DATA *obj;
      OBJ_DATA *obj_best;
      int max;

      max = 1;
      obj_best = NULL;
      for( obj = ch->in_room->first_content; obj; obj = obj->next_content )
      {
        if( CAN_WEAR( obj, ITEM_TAKE ) && obj->cost > max && !IS_OBJ_STAT( obj, ITEM_BURIED ) )
        {
          obj_best = obj;
          max = obj->cost;
        }
      }

      if( obj_best )
      {
        obj_from_room( obj_best );
        obj_to_char( obj_best, ch );
        act( AT_ACTION, "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
      }
    }

    /*
     * Wander 
     */
    if( !xIS_SET( ch->act, ACT_RUNNING )
        && !xIS_SET( ch->act, ACT_SENTINEL )
        && !xIS_SET( ch->act, ACT_PROTOTYPE )
        && ( door = number_bits( 5 ) ) <= 9
        && ( pexit = get_exit( ch->in_room, door ) ) != NULL
        && pexit->to_room
        && !IS_SET( pexit->exit_info, EX_CLOSED )
        && !xIS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
        && !xIS_SET( pexit->to_room->room_flags, ROOM_DEATH )
        && ( !xIS_SET( ch->act, ACT_STAY_AREA ) || pexit->to_room->area == ch->in_room->area ) )
    {
      retcode = move_char( ch, pexit, 0 );
      /*
       * If ch changes position due
       * to it's or someother mob's
       * movement via MOBProgs,
       * continue - Kahn 
       */
      if( char_died( ch ) )
        continue;
      if( retcode != rNONE || xIS_SET( ch->act, ACT_SENTINEL ) || ch->position < POS_STANDING )
        continue;
    }

    /*
     * Flee 
     */
    if( ch->hit < ch->max_hit / 2
        && ( door = number_bits( 4 ) ) <= 9
        && ( pexit = get_exit( ch->in_room, door ) ) != NULL
        && pexit->to_room && !IS_SET( pexit->exit_info, EX_CLOSED ) && !xIS_SET( pexit->to_room->room_flags, ROOM_NO_MOB ) )
    {
      CHAR_DATA *rch;
      bool found;

      found = FALSE;
      for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
      {
        if( is_fearing( ch, rch ) )
        {
          switch ( number_bits( 2 ) )
          {
            case 0:
              sprintf( buf, "Get away from me, %s!", rch->name );
              break;
            case 1:
              sprintf( buf, "Leave me be, %s!", rch->name );
              break;
            case 2:
              sprintf( buf, "%s is trying to kill me!  Help!", rch->name );
              break;
            case 3:
              sprintf( buf, "Someone save me from %s!", rch->name );
              break;
          }
          do_yell( ch, buf );
          found = TRUE;
          break;
        }
      }
      if( found )
        retcode = move_char( ch, pexit, 0 );
    }
  }

  return;
}

/* For the message if all seven dragonballs are in the same
   room. - Karma */

void dball_check( void )
{
  CHAR_DATA *ch;

  if( summon_room != NULL )
    return;

  for( ch = first_char; ch; ch = ch->next )
  {
    if( IS_NPC( ch ) )
      continue;
    if( !ch->desc )
      continue;
    if( !ch->pcdata )
      continue;

    if( seven_dballs_here( ch, "earth" ) || seven_dballs_here( ch, "namek" ) )
      ch_printf( ch, "&YThe Seven Dragonballs pulse with light in unison.\n\r" );
    else
      continue;
  }
}

bool is_super_saiyan( CHAR_DATA * ch )
{
  if( xIS_SET( ch->affected_by, AFF_SSJ ) )
    return TRUE;
  if( xIS_SET( ch->affected_by, AFF_SSJ2 ) )
    return TRUE;
  if( xIS_SET( ch->affected_by, AFF_SSJ3 ) )
    return TRUE;
    if( xIS_SET( ch->affected_by, AFF_SSJ4 ) )
      return TRUE;
      if( xIS_SET( ch->affected_by, AFF_SSJGOD ) )
        return TRUE;
  if( xIS_SET( ch->affected_by, AFF_USSJ ) )
    return TRUE;
  if( xIS_SET( ch->affected_by, AFF_USSJ2 ) )
    return TRUE;
  else
    return FALSE;
}

bool is_fighting( CHAR_DATA * ch  )
{
  if( who_fighting( ch ) == NULL )
    return FALSE;
  else
    return TRUE;
}

/*
   This is the check to see if people are in a transformation
   and determains how much mana is subtracted when it hits. -Karn 05.31.05
*/
void mana_check(void)
{
    CHAR_DATA *ch;

    for (ch = first_char; ch; ch = ch->next)
    {
        if (IS_NPC(ch) || !ch->desc || !ch->pcdata)
            continue;

        int minMANA = 50;

        if (is_super_saiyan(ch) && ch->mana <= minMANA && ch->exp < ch->pl)
        {
            act(AT_WHITE, "Your energy has become too weak to sustain your transformation.", ch, NULL, NULL, TO_CHAR);
            do_powerdown(ch, ch->name);
            continue;
        }

        // Super Saiyan God drain logic
        if (xIS_SET(ch->affected_by, AFF_SSJGOD) && is_fighting(ch))
        {
            int skill = ch->pcdata->learned[gsn_ssjgod];
            if (skill < 100)
            {
                int drain = (int)((1000.0 * (100 - skill)) / 100.0);
                ch->mana -= UMIN(drain, ch->mana);
            }
        }

        // Other SSJ tier drain logic
        if (xIS_SET(ch->affected_by, AFF_SSJ) && is_fighting(ch))
        {
            int skill = ch->pcdata->learned[gsn_ssj];
            if (skill < 100)
            {
                int drain = (int)((100.0 * (100 - skill)) / 100.0);
                ch->mana -= UMIN(drain, ch->mana);
            }
        }
        if (xIS_SET(ch->affected_by, AFF_SSJ2) && is_fighting(ch))
        {
            int skill = ch->pcdata->learned[gsn_ssj2];
            if (skill < 100)
            {
                int drain = (int)((400.0 * (100 - skill)) / 100.0);
                ch->mana -= UMIN(drain, ch->mana);
            }
        }
        if (xIS_SET(ch->affected_by, AFF_SSJ3) && is_fighting(ch))
        {
            int skill = ch->pcdata->learned[gsn_ssj3];
            if (skill < 100)
            {
                int drain = (int)((700.0 * (100 - skill)) / 100.0);
                ch->mana -= UMIN(drain, ch->mana);
            }
        }
        if (xIS_SET(ch->affected_by, AFF_SSJ4) && is_fighting(ch))
        {
            int skill = ch->pcdata->learned[gsn_ssj4];
            if (skill < 100)
            {
                int drain = (int)((100.0 * (100 - skill)) / 100.0);
                ch->mana -= UMIN(drain, ch->mana);
            }
        }
        if (xIS_SET(ch->affected_by, AFF_USSJ) && is_fighting(ch))
        {
            int skill = ch->pcdata->learned[gsn_ussj];
            if (skill < 100)
            {
                int drain = (int)((300.0 * (100 - skill)) / 100.0);
                ch->mana -= UMIN(drain, ch->mana);
            }
        }
        if (xIS_SET(ch->affected_by, AFF_USSJ2) && is_fighting(ch))
        {
            int skill = ch->pcdata->learned[gsn_ussj2];
            if (skill < 100)
            {
                int drain = (int)((600.0 * (100 - skill)) / 100.0);
                ch->mana -= UMIN(drain, ch->mana);
            }
        }

        if (get_trust(ch) > 65)
        {
            send_to_char("Your level is too high. Please log off now.\n", ch);
        }
    }
}

void summon_update( void )
{
  CHAR_DATA *ch;
  CHAR_DATA *dragon;
  MOB_INDEX_DATA *pMobIndex;
  OBJ_DATA *obj;
  OBJ_DATA *obj2;
  OBJ_DATA *obj3;
  OBJ_DATA *obj4;
  OBJ_DATA *obj5;
  OBJ_DATA *obj6;
  OBJ_DATA *obj7;
  char buf[MAX_STRING_LENGTH];

  if( summon_state == SUMMON_NONE )
    return;

  if( summon_state == SUMMON_SKY1 )
  {
    sprintf( buf, "&zBlack clouds suddenly begin to fill the sky all around." );
    echo_to_area( summon_area, buf );
    summon_state = SUMMON_SKY2;
    return;
  }
  if( summon_state == SUMMON_SKY2 )
  {
    sprintf( buf, "&zClaps of thunder rumble through the blackened skies." );
    echo_to_area( summon_area, buf );
    summon_state = SUMMON_RISE1;
    return;
  }
  if( summon_state == SUMMON_RISE1 )
  {
    sprintf( buf, "&YA column of bright, golden energy suddenly explodes upward into the sky from the seven dragonballs." );
    echo_to_room( AT_YELLOW, summon_room, buf );
    summon_state = SUMMON_RISE2;
    return;
  }
  if( summon_state == SUMMON_RISE2 )
  {
    sprintf( buf,
             "&YThe column of bright, golden energy grows larger, twisting and weaving through the sky as it takes shape." );
    echo_to_room( AT_YELLOW, summon_room, buf );
    summon_state = SUMMON_FINISHED1;
    return;
  }
  if( summon_state == SUMMON_FINISHED1 )
  {
    pMobIndex = get_mob_index( 1234 );
    dragon = create_mobile( pMobIndex );
    char_to_room( dragon, summon_room );

    sprintf( buf,
             "&gThe light subsides, revealing a collossal, green serpent dragon floating ominously in the sky above; looking down at you." );
    echo_to_room( AT_DGREEN, summon_room, buf );
    sprintf( buf, "&cA deep, slow, rumbling growl seethes from the dragon's mouth as it breathes." );
    echo_to_room( AT_CYAN, summon_room, buf );
    summon_state = SUMMON_FINISHED2;
    return;
  }
  if( summon_state == SUMMON_FINISHED2 )
  {
    sprintf( buf,
             "&CThe Eternal Dragon 'Shenron' says in a commanding voice 'I am the Eternal Dragon. I will grant you one wish, and one wish only. Now, tell me what it is you wish me to grant you.'" );
    echo_to_room( AT_LBLUE, summon_room, buf );
    sprintf( buf, "&RShenron's eyes turn bright red." );
    echo_to_room( AT_RED, summon_room, buf );
    summon_state = SUMMON_NONE;
    return;
  }
  if( summon_state == SUMMON_LEAVE1 )
  {
    sprintf( buf,
             "&CThe Eternal Dragon 'Shenron' says in a commanding voice 'Your wish has been granted. And now, I bid you farewell." );
    echo_to_room( AT_LBLUE, summon_room, buf );
    sprintf( buf,
             "&YShenron turns bright gold and vanishes, the dragonballs suddenly blazing with incredible golden energy." );
    echo_to_room( AT_YELLOW, summon_room, buf );
    summon_state = SUMMON_LEAVE2;
    return;
  }
  if( summon_state == SUMMON_LEAVE2 )
  {
    sprintf( buf,
             "&YThe dragonballs suddenly shoot straight up into the sky and stop. The dragonballs flash once, then rocket off in random directions; disappearing into the far distance." );
    echo_to_area( summon_area, buf );
    ch = summon_room->first_person;
    if( !ch )
    {
      summon_state = SUMMON_NONE;
      bug( "Summon_update: Nobody in room", 0 );
      return;
    }
    dragon = get_char_room( ch, "shenron" );
    if( !dragon )
    {
      summon_state = SUMMON_NONE;
      bug( "Summon_update: Dragon not in room", 0 );
      return;
    }
    if( ( obj = get_obj_vnum_room( dragon, 1280 ) ) != NULL )
    {
      obj_from_room( obj );
      extract_obj( obj );
    }
    if( ( obj2 = get_obj_vnum_room( dragon, 1281 ) ) != NULL )
    {
      obj_from_room( obj2 );
      extract_obj( obj2 );
    }
    if( ( obj3 = get_obj_vnum_room( dragon, 1282 ) ) != NULL )
    {
      obj_from_room( obj3 );
      extract_obj( obj3 );
    }
    if( ( obj4 = get_obj_vnum_room( dragon, 1283 ) ) != NULL )
    {
      obj_from_room( obj4 );
      extract_obj( obj4 );
    }
    if( ( obj5 = get_obj_vnum_room( dragon, 1284 ) ) != NULL )
    {
      obj_from_room( obj5 );
      extract_obj( obj5 );
    }
    if( ( obj6 = get_obj_vnum_room( dragon, 1285 ) ) != NULL )
    {
      obj_from_room( obj6 );
      extract_obj( obj6 );
    }
    if( ( obj7 = get_obj_vnum_room( dragon, 1286 ) ) != NULL )
    {
      obj_from_room( obj7 );
      extract_obj( obj7 );
    }
    xREMOVE_BIT( summon_room->room_flags, ROOM_SAFE );
    extract_char( dragon, TRUE );
    summon_state = SUMMON_NONE;
    summon_area = NULL;
    summon_room = NULL;
    return;
  }
  return;
}

/* For handling mystic training - Karma */

void mystic_check( void )
{
  CHAR_DATA *ch, *ch_next;

  for( ch = first_char; ch; ch = ch_next )
  {
    ch_next = ch->next;
    if( !ch->teaching )
      continue;
    if( ch->tmystic == 0 )
      continue;

    if( ch->mysticlearn > 0 )
      ch->mysticlearn--;

    if( ch->mysticlearn == 15 )
    {
      act( AT_WHITE,
           "You finish dancing infront of $N, and sit down on the ground. You then throw your hands out infront of $N and say \"BLUAGH\" as you start into the second half of the mystic training.",
           ch->teaching, NULL, ch, TO_CHAR );
      act( AT_WHITE,
           "$n finishes dancing infront of you, and sits down on the ground. $n then throws $s hands out infront of you and says \"BLUAGH\" as $e starts into the second half of the mystic training.",
           ch->teaching, NULL, ch, TO_VICT );
      act( AT_WHITE,
           "$n finishes dancing infront of $N, and sits down on the ground. $n then throws $s hands out infront of $N and says \"BLUAGH\" as $e starts into the second half of the mystic training.",
           ch->teaching, NULL, ch, TO_NOTVICT );
      ch_printf( ch, "&RYou still may not type anything. You still have 15 minutes left to go.\n\r" );
      ch_printf( ch->teaching, "&RYou still may not type anything. You still have 15 minutes left to go.\n\r" );
      ch->tmystic = 3;
      continue;
    }
    else if( ch->mysticlearn > 15 )
    {
      act( AT_WHITE, "You continue dancing in a large circle around $N and chanting random gibberish.", ch->teaching, NULL,
           ch, TO_CHAR );
      act( AT_WHITE, "$n continues dancing in a large circle around you and chanting random gibberish.", ch->teaching, NULL,
           ch, TO_VICT );
      act( AT_WHITE, "$n continues dancing in a large circle around $N and chanting random gibberish.", ch->teaching, NULL,
           ch, TO_NOTVICT );
      continue;
    }
    else if( ch->mysticlearn < 15 && ch->mysticlearn > 0 )
    {
      act( AT_WHITE,
           "You continue sitting infront of $N with your palms facing $e, as you concentrate on releasing $s hidden powers.",
           ch->teaching, NULL, ch, TO_CHAR );
      act( AT_WHITE,
           "$n continues sitting infront of you with $s palms facing you, as $e concentrates on releasing your hidden powers.",
           ch->teaching, NULL, ch, TO_VICT );
      act( AT_WHITE,
           "$n continues sitting infront of $N with $s palms facing $N, as $e concentrates on releasing $N's hidden powers.",
           ch->teaching, NULL, ch, TO_NOTVICT );
      continue;
    }
    else if( ch->mysticlearn == 0 && ch->tmystic == 3 )
    {
      ch->tmystic = 0;
      ch->teaching->tmystic = 0;
      act( AT_WHITE, "$n suddenly tells you that you are done. You have successfully learned mystic.", ch->teaching, NULL,
           ch, TO_VICT );
      act( AT_WHITE, "You tell $N that you are done. You have successfully taught $N mystic.", ch->teaching, NULL, ch,
           TO_CHAR );
      act( AT_WHITE, "$n suddenly tells $N that $e is done. $N has successfully learned mystic.", ch->teaching, NULL, ch,
           TO_NOTVICT );
      ch->pcdata->learned[gsn_mystic] = 95;
      SET_BIT( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC );
      ch->teaching->teaching = NULL;
      ch->teaching = NULL;
      save_char_obj( ch );
      do_powerup( ch, "" );
      do_powerdown( ch, "" );
      do_mystic( ch, "20" );
      continue;
    }
  }
}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_save;
  sh_int save_count = 0;
  sh_int tIdle = 0;
  ch_save = NULL;

  for( ch = last_char; ch; ch = gch_prev )
  {
    if( ch == first_char && ch->prev )
    {
      bug( "char_update: first_char->prev != NULL... fixed", 0 );
      ch->prev = NULL;
    }
    gch_prev = ch->prev;
    set_cur_char( ch );
    if( gch_prev && gch_prev->next != ch )
    {
      bug( "char_update: ch->prev->next != ch", 0 );
      return;
    }

    /*
     *  Do a room_prog rand check right off the bat
     *   if ch disappears (rprog might wax npc's), continue
     */
    if( !IS_NPC( ch ) )
      rprog_random_trigger( ch );

    if( char_died( ch ) )
      continue;

    if( IS_NPC( ch ) )
      mprog_time_trigger( ch );

    if( char_died( ch ) )
      continue;

    rprog_time_trigger( ch );

    if( char_died( ch ) )
      continue;

    /*
     * See if player should be auto-saved.
     */
    if( !IS_NPC( ch )
        && ( !ch->desc || ch->desc->connected == CON_PLAYING )
        && current_time - ch->save_time > ( sysdata.save_frequency * 60 ) )
      ch_save = ch;
    else
      ch_save = NULL;

    if( IS_NPC( ch ) && is_splitformed( ch ) && !ch->master )
    {
      extract_char( ch, TRUE );
      continue;
    }

    /*
     * If ch is link-dead stop running stuff to prevent people
     * from going link-dead to avoid pk timers or healing while
     * playing another character or anything else
     */
    if( !IS_NPC( ch ) && !ch->desc )
      continue;

    if( ch->position >= POS_DEAD )
    {
      if( ch->hit < ch->max_hit )
        ch->hit += hit_gain( ch );

      if( ch->mana < ch->max_mana )
        ch->mana += mana_gain( ch );

//      if ( ch->move < ch->max_move )
//    ch->move += move_gain(ch);

      if( !IS_NPC( ch ) )
        ch->pcdata->natural_ac = ( float )( ch->hit / 100 ) * ch->pcdata->natural_ac_max;

    }

    if( ch->hit > ch->max_hit )
      ch->hit = ch->max_hit;

    heart_calc( ch, "" );

    if( !who_fighting( ch ) && ch->focus != 0 )
    {
      ch->focus -= URANGE( 0, 20, get_curr_int( ch ) );
      ch->focus = URANGE( 0, ch->focus, get_curr_int( ch ) );
    }

    if( ch->position != POS_DEAD )
      update_pos( ch );

    /*
     * Morph timer expires 
     */

    if( ch->morph )
    {
      if( ch->morph->timer > 0 )
      {
        ch->morph->timer--;
        if( ch->morph->timer == 0 )
          do_unmorph_char( ch );
      }
    }

    /*
     * To make people with a nuisance's flags life difficult
     * * --Shaddai
     */

    if( !IS_NPC( ch ) && ch->pcdata->nuisance )
    {
      long int temp;

      if( ch->pcdata->nuisance->flags < MAX_NUISANCE_STAGE )
      {
        temp = ch->pcdata->nuisance->max_time - ch->pcdata->nuisance->time;
        temp *= ch->pcdata->nuisance->flags;
        temp /= MAX_NUISANCE_STAGE;
        temp += ch->pcdata->nuisance->time;
        if( temp < current_time )
          ch->pcdata->nuisance->flags++;
      }
    }

    if( !IS_NPC( ch ) )
    {
      if( ch->pcdata->condition[COND_FULL] < 12 )
        ch->pcdata->condition[COND_FULL] = 12;
      if( ch->pcdata->condition[COND_THIRST] < 12 )
        ch->pcdata->condition[COND_THIRST] = 12;
      if( ch->mental_state < 0 )
        ch->mental_state = 0;
    }

    if( !IS_NPC( ch ) && ch->level < LEVEL_IMMORTAL )
    {
      OBJ_DATA *obj;

      if( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] > 0 )
      {
        if( --obj->value[2] == 0 && ch->in_room )
        {
          ch->in_room->light -= obj->count;
          if( ch->in_room->light < 0 )
            ch->in_room->light = 0;
          act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_ROOM );
          act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_CHAR );
          if( obj->serial == cur_obj )
            global_objcode = rOBJ_EXPIRED;
          extract_obj( obj );
        }
      }

      if( ch->tmystic == 0 )
        ch->timer++;
      if( ch->timer >= 12 )
      {
        if( !IS_IDLE( ch ) )
        {
          /*
           * ch->was_in_room = ch->in_room;
           */
          if( ch->fighting )
            stop_fighting( ch, TRUE );
          act( AT_ACTION, "$n disappears into the void.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You disappear into the void.\n\r", ch );
          if( IS_SET( sysdata.save_flags, SV_IDLE ) )
            save_char_obj( ch );
          SET_BIT( ch->pcdata->flags, PCFLAG_IDLE );
          char_from_room( ch );
          char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
        }
      }

      if( ch->pcdata->bot_warn[0] >= 5 )
      {
        ch->pcdata->bot_warn[1] = 0;
        ch->pcdata->bot_warn[0] = 0;

        tIdle =
          ch->pcdata->pIdle[0] + ch->pcdata->pIdle[1] + ch->pcdata->pIdle[2] + ch->pcdata->pIdle[3] + ch->pcdata->pIdle[4];
        tIdle = tIdle / 5;

        if( ch->pcdata->pIdle[0] <= ( tIdle + 5 ) && ch->pcdata->pIdle[0] >= ( tIdle - 5 ) )
          ch->pcdata->bot_warn[1]++;
        if( ch->pcdata->pIdle[1] <= ( tIdle + 5 ) && ch->pcdata->pIdle[1] >= ( tIdle - 5 ) )
          ch->pcdata->bot_warn[1]++;
        if( ch->pcdata->pIdle[2] <= ( tIdle + 5 ) && ch->pcdata->pIdle[2] >= ( tIdle - 5 ) )
          ch->pcdata->bot_warn[1]++;
        if( ch->pcdata->pIdle[3] <= ( tIdle + 5 ) && ch->pcdata->pIdle[3] >= ( tIdle - 5 ) )
          ch->pcdata->bot_warn[1]++;
        if( ch->pcdata->pIdle[4] <= ( tIdle + 5 ) && ch->pcdata->pIdle[4] >= ( tIdle - 5 ) )
          ch->pcdata->bot_warn[1]++;

        if( ch->pcdata->bot_warn[1] >= 5 )
          ch->pcdata->bot_warn[2]++;
      }




      if( ch->pcdata->condition[COND_DRUNK] > 8 )
        worsen_mental_state( ch, ch->pcdata->condition[COND_DRUNK] / 8 );


      if( ch->pcdata->condition[COND_FULL] > 1 )
      {
        switch ( ch->position )
        {
          case POS_SLEEPING:
            better_mental_state( ch, 4 );
            break;
          case POS_RESTING:
            better_mental_state( ch, 3 );
            break;
          case POS_SITTING:
          case POS_MOUNTED:
            better_mental_state( ch, 2 );
            break;
          case POS_STANDING:
            better_mental_state( ch, 1 );
            break;
          case POS_FIGHTING:
          case POS_EVASIVE:
          case POS_DEFENSIVE:
          case POS_AGGRESSIVE:
          case POS_BERSERK:
            if( number_bits( 2 ) == 0 )
              better_mental_state( ch, 1 );
            break;
        }
      }
      if( ch->pcdata->condition[COND_THIRST] > 1 )
      {
        switch ( ch->position )
        {
          case POS_SLEEPING:
            better_mental_state( ch, 5 );
            break;
          case POS_RESTING:
            better_mental_state( ch, 3 );
            break;
          case POS_SITTING:
          case POS_MOUNTED:
            better_mental_state( ch, 2 );
            break;
          case POS_STANDING:
            better_mental_state( ch, 1 );
            break;
          case POS_FIGHTING:
          case POS_EVASIVE:
          case POS_DEFENSIVE:
          case POS_AGGRESSIVE:
          case POS_BERSERK:
            if( number_bits( 2 ) == 0 )
              better_mental_state( ch, 1 );
            break;
        }
      }

/*	    check_alignment( ch );
*/
      gain_condition( ch, COND_DRUNK, -1 );
      if( ch->mental_state != 0 )
      {
        switch ( ch->position )
        {
          case POS_SLEEPING:
            better_mental_state( ch, 5 );
            break;
          case POS_RESTING:
            better_mental_state( ch, 3 );
            break;
          case POS_SITTING:
          case POS_MOUNTED:
            better_mental_state( ch, 2 );
            break;
          case POS_STANDING:
            better_mental_state( ch, 1 );
            break;
          case POS_FIGHTING:
          case POS_EVASIVE:
          case POS_DEFENSIVE:
          case POS_AGGRESSIVE:
          case POS_BERSERK:
            if( number_bits( 2 ) == 0 )
              better_mental_state( ch, 1 );
            break;
        }
      }

      gain_condition( ch, COND_FULL, -1 + race_table[ch->race]->hunger_mod );
/*

	    if ( CAN_PKILL( ch ) && ch->pcdata->condition[COND_THIRST] - 9 > 10 )
	      gain_condition( ch, COND_THIRST, -9 );

*/
      if( !IS_NPC( ch ) && ch->pcdata->nuisance )
      {
        int value;

        value = ( ( 0 - ch->pcdata->nuisance->flags ) * ch->pcdata->nuisance->power );
        gain_condition( ch, COND_THIRST, value );
        gain_condition( ch, COND_FULL, --value );
      }

      if( ch->in_room )
        switch ( ch->in_room->sector_type )
        {
          default:
            gain_condition( ch, COND_THIRST, -1 + race_table[ch->race]->thirst_mod );
            break;
          case SECT_DESERT:
            gain_condition( ch, COND_THIRST, -3 + race_table[ch->race]->thirst_mod );
            break;
          case SECT_UNDERWATER:
          case SECT_OCEANFLOOR:
            if( number_bits( 1 ) == 0 )
              gain_condition( ch, COND_THIRST, -1 + race_table[ch->race]->thirst_mod );
            break;
        }

    }

    if( !IS_NPC( ch ) )
    {
      clan_auto_kick( ch );

      if( ch->pcdata->pk_timer > 0 && ch->desc )
      {
        ch->pcdata->pk_timer--;
      }
      else if( xIS_SET( ch->act, PLR_PK1 ) || xIS_SET( ch->act, PLR_PK2 ) || xIS_SET( ch->act, PLR_WAR2 ) )
      {
        if( ch->pcdata->pk_timer != 0 )
          ch->pcdata->pk_timer++;
        else
        {
          ch->pcdata->pk_timer = -15;
          send_to_pager_color( "&wYou can now turn off your PK flags.\r\n", ch );
        }
      }

      if( ch->pcdata->gohometimer != 0 )
        ch->pcdata->gohometimer--;

      if( ch->pcdata->eKTimer > 0 )
        ch->pcdata->eKTimer -= 1;

      if( xIS_SET( ch->act, PLR_SILENCE ) && ch->pcdata->silence != -1 )
      {
        if( ch->pcdata->silence > 0 )
          ch->pcdata->silence--;
        else
        {
          xREMOVE_BIT( ch->act, PLR_SILENCE );
          set_char_color( AT_IMMORT, ch );
          send_to_pager_color( "&wYou may now use public channels again.\r\n", ch );
          ch->pcdata->silence = 0;
          STRFREE( ch->pcdata->silencedby );
        }
      }

      if( ch->pcdata->b_timeleft > 0 )
        ch->pcdata->b_timeleft--;
      if( ch->pcdata->b_timeleft < 0 )
        ch->pcdata->b_timeleft = 0;

      if( ch->pcdata->tail < 0 )
        ch->pcdata->tail++;
      else if( ch->pcdata->tail == 0 && ( is_saiyan( ch ) || is_hb( ch ) ) )
      {
        /*
         * Added to make halfie tail regen less
         * likely. -Karma
         */
        bool skip = FALSE;
        if( is_hb( ch ) )
        {
          if( number_range( 1, 2 ) == 2 )
            skip = TRUE;
        }
        if( !skip )
        {
          act( AT_GREEN, "Your tail has grown back!", ch, NULL, NULL, TO_CHAR );
          act( AT_GREEN, "$n's tail has grown back!", ch, NULL, NULL, TO_ROOM );
          ch->pcdata->tail = 1;
        }
      }
    }

    if( !IS_NPC( ch ) && ch->pcdata->nextHBTCDate > 0 && ch->pcdata->nextHBTCDate <= current_time )
    {
      ch->pcdata->nextHBTCDate = 0;
      ch->pcdata->HBTCTimeLeft = 0;
      send_to_char( "You can enter the Hyperbolic Time Chamber once again.\n\r", ch );
    }

    if( !IS_NPC( ch ) && ch->pcdata->nextspartime > 0 && ch->pcdata->nextspartime <= current_time )
    {
      ch->pcdata->nextspartime = 0;
      ch->pcdata->sparcount = 0;
      send_to_char( "You have fully recovered from sparring yesterday.\n\r", ch );
    }

    if( !IS_NPC( ch )
        && ch->pcdata->HBTCTimeLeft == 1 && xIS_SET( ch->in_room->room_flags, ROOM_TIME_CHAMBER ) && !IS_IMMORTAL( ch ) )
    {
      act( AT_ACTION, "Your time in the Hyperbolic Time Chamber is almost up!", ch, NULL, NULL, TO_CHAR );
    }

    if( !IS_NPC( ch )
        && ch->pcdata->HBTCTimeLeft <= 0 && xIS_SET( ch->in_room->room_flags, ROOM_TIME_CHAMBER ) && !IS_IMMORTAL( ch ) )
    {
      ROOM_INDEX_DATA *location;


      location = get_room_index( 2059 );
      if( !location )
        location = get_room_index( ROOM_VNUM_TEMPLE );

      send_to_char( "Your time in the Hyperbolic Time Chamber has ended.\n\r", ch );
      act( AT_GREEN, "$n's time in the Hyperbolic Time Chamber has ended.", ch, NULL, NULL, TO_ROOM );
      char_from_room( ch );
      char_to_room( ch, location );
      if( ch->fighting )
        stop_fighting( ch, TRUE );
      do_look( ch, "auto" );
      save_char_obj( ch );
    }

    if( !IS_NPC( ch ) && ch->fusiontimer == 1 && IS_SET( ch->fusionflags, FUSION_DANCE ) && !IS_IMMORTAL( ch ) )
    {
      act( AT_ACTION, "Your time in the Fusion time is almost up!", ch, NULL, NULL, TO_CHAR );
    }


    if( !IS_NPC( ch ) && ch->fusiontimer <= 0 && IS_SET( ch->fusionflags, FUSION_DANCE ) && !IS_IMMORTAL( ch ) )
    {

      send_to_char( "Your fusion dance time has ended.\n\r", ch );
      act( AT_GREEN, "$n's fusion time has ended.", ch, NULL, NULL, TO_ROOM );

      if( ch->fighting )
        stop_fighting( ch, TRUE );
      do_defuse( ch, "" );
      save_char_obj( ch );
    }


    if( !IS_NPC( ch ) && ch->pcdata->gnote_date > 0 && ch->pcdata->gnote_date <= current_time )
    {
      ch->pcdata->gnote_date = 0;
      xREMOVE_BIT( ch->act, PLR_NOGBOARD );
      send_to_char( "You can use gnotes again.\n\r", ch );
    }

    if( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ch->pcdata->release_date > 0 && ch->pcdata->release_date <= current_time )
    {
      ROOM_INDEX_DATA *location;
/*
         if ( ch->pcdata->clan )
          location = get_room_index( ch->pcdata->clan->recall );
         else
*/
      location = get_room_index( ROOM_VNUM_TEMPLE );
      if( !location )
        location = ch->in_room;
      MOBtrigger = FALSE;
      char_from_room( ch );
      char_to_room( ch, location );
      send_to_char( "The gods have released you from hell as your sentance is up!\n\r", ch );
      do_look( ch, "auto" );
      STRFREE( ch->pcdata->helled_by );
      ch->pcdata->helled_by = NULL;
      ch->pcdata->release_date = 0;
      save_char_obj( ch );
    }

    if( !IS_NPC( ch ) )
      mccp_interest( ch );

    if( !IS_NPC( ch ) && !IS_IMMORTAL( ch )
        && ( ch->in_room->vnum == 6 || ch->in_room->vnum == 8 ) && ch->pcdata->release_date == 0 )
    {
      ROOM_INDEX_DATA *location;
      location = get_room_index( ROOM_VNUM_TEMPLE );
      if( !location )
        location = ch->in_room;
      MOBtrigger = FALSE;
      char_from_room( ch );
      char_to_room( ch, location );
      do_look( ch, "auto" );
      return;
    }

    if( !IS_NPC( ch ) && ch->pcdata->admintalk > 0 )
      ch->pcdata->admintalk -= 1;

    if( !IS_NPC( ch ) && ch->pcdata->HBTCTimeLeft > 0 && xIS_SET( ch->in_room->room_flags, ROOM_TIME_CHAMBER ) )
      ch->pcdata->HBTCTimeLeft -= 1;

    if( number_percent(  ) < 25 && !IS_IMMORTAL( ch ) )
    {
      if( IS_AFFECTED( ch, AFF_SSJ ) && !IS_AFFECTED( ch, AFF_SSJ2 )
          && !IS_AFFECTED( ch, AFF_SSJ3 ) && !IS_AFFECTED( ch, AFF_SSJ4 ) )
      {
        act( AT_YELLOW, "The yellow aura around your body hums softly.", ch, NULL, NULL, TO_CHAR );
        act( AT_YELLOW, "The yellow aura around $n's body hums softly.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_SSJ ) && IS_AFFECTED( ch, AFF_SSJ2 )
          && !IS_AFFECTED( ch, AFF_SSJ3 ) && !IS_AFFECTED( ch, AFF_SSJ4 ) )
      {
        switch ( number_range( 1, 3 ) )
        {
          case 1:
            if( IS_AFFECTED( ch, AFF_MAJIN ) )
            {
              act( AT_RED, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_RED, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            else
            {
              act( AT_BLUE, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_BLUE, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            break;
          case 2:
            if( IS_AFFECTED( ch, AFF_MAJIN ) )
            {
              act( AT_RED, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_RED, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            else
            {
              act( AT_BLUE, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_BLUE, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            break;
          case 3:
            if( IS_AFFECTED( ch, AFF_MAJIN ) )
            {
              act( AT_RED, "Red lightning crackles around you, flowing through your yellow aura.", ch, NULL, NULL, TO_CHAR );
              act( AT_RED, "Red lightning crackles around $n, flowing through $s yellow aura.", ch, NULL, NULL, TO_NOTVICT );
            }
            else
            {
              act( AT_BLUE, "Lightning crackles around you, flowing through your yellow aura.", ch, NULL, NULL, TO_CHAR );
              act( AT_BLUE, "Lightning crackles around $n, flowing through $s yellow aura.", ch, NULL, NULL, TO_NOTVICT );
            }
            break;
        }
      }
      if( IS_AFFECTED( ch, AFF_SSJ ) && IS_AFFECTED( ch, AFF_SSJ2 )
          && IS_AFFECTED( ch, AFF_SSJ3 ) && !IS_AFFECTED( ch, AFF_SSJ4 ) )
      {
        switch ( number_range( 1, 3 ) )
        {
          case 1:
            if( IS_AFFECTED( ch, AFF_MAJIN ) )
            {
              act( AT_RED, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_RED, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            else
            {
              act( AT_BLUE, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_BLUE, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            break;
          case 2:
            if( IS_AFFECTED( ch, AFF_MAJIN ) )
            {
              act( AT_RED, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_RED, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            else
            {
              act( AT_BLUE, "Electricity crackles around you.", ch, NULL, NULL, TO_CHAR );
              act( AT_BLUE, "Electricity crackles around $n.", ch, NULL, NULL, TO_NOTVICT );
            }
            break;
          case 3:
            act( AT_YELLOW, "Your mane of hair shifts slightly, your aura pulsating with golden energy.", ch, NULL, NULL,
                 TO_CHAR );
            act( AT_YELLOW, "$n's mane of hair shifts slightly, $s aura pulsating with golden energy.", ch, NULL, NULL,
                 TO_NOTVICT );
            break;
        }
      }
      if( IS_AFFECTED( ch, AFF_SSJ ) && IS_AFFECTED( ch, AFF_SSJ2 )
          && IS_AFFECTED( ch, AFF_SSJ3 ) && IS_AFFECTED( ch, AFF_SSJ4 ) )
      {
        act( AT_RED, "Your aura pulses silently, your furry tail swaying through the air.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "$n's aura pulses silently, $s furry tail swaying through the air.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_SNAMEK ) )
      {
        act( AT_GREEN, "&gThe green flames around you flare up brightly.&G", ch, NULL, NULL, TO_CHAR );
        act( AT_GREEN, "&gThe green flames around $n flare up brightly.&G", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_UNAMEK ) )
      {
        act( AT_GREEN, "The emerald flames around you flare, glowing brighter for a brief moment.", ch, NULL, NULL,
             TO_CHAR );
        act( AT_GREEN, "The emerald flames around $n flare, glowing brighter for a brief moment.", ch, NULL, NULL,
             TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_ICER4 ) || IS_AFFECTED( ch, AFF_ICER5 ) )
      {
        act( AT_BLUE, "Your icy aura sends a freezing cold breeze through the room.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's icy aura sends a freezing cold breeze through the room.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_KAIOKEN ) )
      {
        act( AT_RED, "A hum fills the room as the mystical flames around you tingle your senses.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "A hum fills the room as the mystical flames around $n tingle your senses.", ch, NULL, NULL,
             TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_SUPER_OOZARU ) )
      {
        act( AT_RED, "You growl deeply the ground underneath you shaking.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "$n growls deeply the ground shaking under them.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_HEART ) )
      {
        act( AT_RED, "The pounding of your heartbeat echoes throughout the room.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "The pounding of $n's heartbeat echoes throughout the room.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_ELECTRICSHIELD ) )
      {
        act( AT_BLUE, "The shield of electricity around you crackles and hums.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "The shield of electricity around $n crackles and hums.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_HYPER ) )
      {
        act( AT_WHITE, "You flex your bulging muscles, veins throbbing as pure energy courses through your body.", ch, NULL,
             NULL, TO_CHAR );
        act( AT_WHITE, "$n flexes $s bulging muscles, veins throbbing as pure energy courses through $s body.", ch, NULL,
             NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_EXTREME ) )
      {
        act( AT_WHITE, "The aura around you crackles with bolts of trancendent power.", ch, NULL, NULL, TO_CHAR );
        act( AT_WHITE, "The aura around $n crackles with bolts of trancendent power.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_SEMIPERFECT ) )
      {
        act( AT_PURPLE, "Your aura flashes purple, black lightning crackling before the energy melds back together.", ch,
             NULL, NULL, TO_CHAR );
        act( AT_PURPLE, "$n's aura flashes purple, black lightning crackling before the energy melds back together.", ch,
             NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_PERFECT ) )
      {
        act( AT_DGREY, "The energy around you flickers and dances, the wailing of dead souls filling the air.", ch, NULL,
             NULL, TO_CHAR );
        act( AT_DGREY, "The energy around $n flickers and dances, the wailing of dead souls filling the air.", ch, NULL,
             NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_ULTRAPERFECT ) )
      {
        act( AT_DGREY, "The black energy around you crackles through your stolen aura, drawing it momentarily "
             "closer to your form.", ch, NULL, NULL, TO_CHAR );
        act( AT_DGREY, "The black energy around $n crackles through $s stolen aura, drawing it momentarily "
             "closer to $s form.", ch, NULL, NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_MYSTIC ) )
      {

        act( get_aura( ch ), "The innermost power of yourself is revealed as shockwaves emanate from your body.", ch, NULL,
             NULL, TO_CHAR );
        act( get_aura( ch ), "The innermost power of $n is revealed as shockwaves emanate from $s body.", ch, NULL, NULL,
             TO_NOTVICT );
      }

      if( IS_AFFECTED( ch, AFF_LSSJ ) )
      {
        if( IS_RINI(ch) )
        {
          act( AT_YELLOW, "&CThe incredible power surrounding you warps space and time about you.", ch, NULL, NULL, TO_CHAR );
          act( AT_YELLOW, "&CThe incredible power surrounding Daralys warps space and time about her.", ch, NULL, NULL, TO_NOTVICT );
          act( AT_YELLOW, "The huge, flaming golden aura about you hums softly with immense power.", ch, NULL, NULL, TO_CHAR );
          act( AT_YELLOW, "The huge, flaming golden aura about $n hums softly with immense power.", ch, NULL, NULL, TO_NOTVICT );
        }
        else
        {
          act( AT_YELLOW, "The huge, flaming golden aura about you hums softly with immense power.", ch, NULL, NULL, TO_CHAR );
          act( AT_YELLOW, "The huge, flaming golden aura about $n hums softly with immense power.", ch, NULL, NULL, TO_NOTVICT );
        }
      }

      if( IS_AFFECTED( ch, AFF_MAJIN ) )
      {

        act( AT_YELLOW, "&rThe rage built up inside of you releases itself with an earsplitting crack of energy.&D", ch,
             NULL, NULL, TO_CHAR );
        act( AT_YELLOW, "&rThe rage built up inside $n releases itself with an earsplitting crack of energy.&D", ch, NULL,
             NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_ESSJ ) )
      {

        act( AT_RED, "Lightning flashs as it flows through your corona of energy.&D", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "Lightning flashs as it flows through $n's corona of energy.&D", ch, NULL, NULL, TO_NOTVICT );
      }
      if (IS_AFFECTED(ch, AFF_SSJGOD))
{
    act(AT_RED, "Your divine aura flickers with incandescent fire, lightning crackling violently around you as the heavens seem to tremble.", ch, NULL, NULL, TO_CHAR);
    act(AT_RED, "$n's divine aura flickers with incandescent fire, lightning crackling violently around $m as the heavens seem to tremble.", ch, NULL, NULL, TO_NOTVICT);
}
      if( IS_AFFECTED( ch, AFF_HSSJ ) )
      {
        sh_int z = get_aura( ch );

        act( z, "Flames of energy swirl and flare around you momentarily before settling down a little.&D", ch,
             NULL, NULL, TO_CHAR );
        act( z, "Flames of energy swirl and flare around $n momentarily before settling down a little.&D", ch, NULL,
             NULL, TO_NOTVICT );
      }
      if( IS_AFFECTED( ch, AFF_EVILOVERLOAD ) )
      {
        act( AT_DGREY, "The energy surrounding you crackles, as evil power surges through your muscles.&D", ch,
             NULL, NULL, TO_CHAR );
        act( AT_DGREY, "The energy surrounding $n crackles, as evil power surges through $s muscles.&D", ch, NULL,
             NULL, TO_NOTVICT );
      }
    }
    if( !char_died( ch ) )
    {
      /*
       * Careful with the damages here,
       *   MUST NOT refer to ch after damage taken, without checking
       *   return code and/or char_died as it may be lethal damage.
       */
      if( IS_AFFECTED( ch, AFF_POISON ) )
      {
        act( AT_POISON, "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
        act( AT_POISON, "You shiver and suffer.", ch, NULL, NULL, TO_CHAR );
        ch->mental_state = URANGE( 20, ch->mental_state + ( IS_NPC( ch ) ? 2 : IS_PKILL( ch ) ? 3 : 4 ), 100 );
        damage( ch, ch, 6, gsn_poison );
      }
/*
	    else
	    if ( ch->position == POS_INCAP )
		damage( ch, ch, 1, TYPE_UNDEFINED );
	    else
	    if ( ch->position == POS_MORTAL )
		damage( ch, ch, 4, TYPE_UNDEFINED );
*/
      if( char_died( ch ) )
        continue;

      /*
       * oozaru! 
       */
      if( !IS_NPC( ch ) )
      {
        if( ch->pcdata->tail > 0 && ch->position >= POS_SLEEPING
            && !xIS_SET( ( ch )->affected_by, AFF_SSJ )
            && !xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
            && !xIS_SET( ( ch )->affected_by, AFF_SSJ3 )
            && !xIS_SET( ( ch )->affected_by, AFF_SSJ4 )
            && !xIS_SET( ( ch )->affected_by, AFF_SSJGOD )
            && !xIS_SET( ( ch )->affected_by, AFF_MYSTIC )
            && !xIS_SET( ( ch )->affected_by, AFF_LSSJ ) && !xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )

        {
          if( ( is_saiyan( ch ) || is_hb( ch ) )
              && ( ( time_info.hour >= 20 && time_info.day == 29 ) || ( time_info.hour <= 5 && time_info.day == 0 ) ) )
          {
            if( !xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) && ch->pcdata->learned[gsn_ssj3] >= 100 && is_saiyan( ch ) )
              transform_golden_oozaru( ch );

            if( !xIS_SET( ch->affected_by, AFF_OOZARU ) &&
                !wearing_chip( ch ) && !xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) )
              transform_oozaru( ch );
          }

          if( ( is_saiyan( ch ) || is_hb( ch ) )
              && ( ( time_info.hour > 5 && time_info.day != 29 ) || ( time_info.hour < 20 && time_info.day != 0 ) ) )
          {
            if( xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) )
              untransform_golden_oozaru( ch );
            if( xIS_SET( ch->affected_by, AFF_OOZARU ) )
              untransform_oozaru( ch );
          }
        }
      }
      if( !IS_NPC( ch ) )
      {
        if( ch->position >= POS_SLEEPING )
        {
          if( IS_DEMON( ch )
              && ( ( time_info.hour >= 20 && time_info.day == 14 ) || ( time_info.hour <= 5 && time_info.day == 15 ) ) )
          {
            if( !xIS_SET( ch->affected_by, AFF_MAKEOSTAR ) && !wearing_chip( ch ) )
              transform_makeo( ch );
          }

          if( IS_DEMON( ch )
              && ( ( time_info.hour > 5 && time_info.day != 14 ) || ( time_info.hour < 20 && time_info.day != 15 ) ) )
          {
            if( xIS_SET( ch->affected_by, AFF_MAKEOSTAR ) )
              untransform_makeo( ch );
          }


        }
      }
      /*
       * Recurring spell affect
       */
      if( IS_AFFECTED( ch, AFF_RECURRINGSPELL ) )
      {
        AFFECT_DATA *paf, *paf_next;
        SKILLTYPE *skill;
        bool found = FALSE, died = FALSE;

        for( paf = ch->first_affect; paf; paf = paf_next )
        {
          paf_next = paf->next;
          if( paf->location == APPLY_RECURRINGSPELL )
          {
            found = TRUE;
            if( IS_VALID_SN( paf->modifier )
                && ( skill = skill_table[paf->modifier] ) != NULL && skill->type == SKILL_SPELL )
            {
              if( ( *skill->spell_fun ) ( paf->modifier, ch->level, ch, ch ) == rCHAR_DIED || char_died( ch ) )
              {
                died = TRUE;
                break;
              }
            }
          }
        }
        if( died )
          continue;
        if( !found )
          xREMOVE_BIT( ch->affected_by, AFF_RECURRINGSPELL );
      }

      if( ch->mental_state >= 30 )
        switch ( ( ch->mental_state + 5 ) / 10 )
        {
          case 3:
            send_to_char( "&gYou feel feverish.\n\r", ch );
            act( AT_ACTION, "$n looks kind of out of it.", ch, NULL, NULL, TO_ROOM );
            break;
          case 4:
            send_to_char( "&gYou do not feel well at all.\n\r", ch );
            act( AT_ACTION, "$n doesn't look too good.", ch, NULL, NULL, TO_ROOM );
            break;
          case 5:
            send_to_char( "&gYou need help!\n\r", ch );
            act( AT_ACTION, "$n looks like $e could use your help.", ch, NULL, NULL, TO_ROOM );
            break;
          case 6:
            send_to_char( "&RSeekest thou a cleric.\n\r", ch );
            act( AT_ACTION, "Someone should fetch a healer for $n.", ch, NULL, NULL, TO_ROOM );
            break;
          case 7:
            send_to_char( "&zYou feel reality slipping away...\n\r", ch );
            act( AT_ACTION, "$n doesn't appear to be aware of what's going on.", ch, NULL, NULL, TO_ROOM );
            break;
          case 8:
            send_to_char( "&gYou begin to understand... everything.\n\r", ch );
            act( AT_ACTION, "$n starts ranting like a madman!", ch, NULL, NULL, TO_ROOM );
            break;
          case 9:
            send_to_char( "&gYou are ONE with the universe.\n\r", ch );
            act( AT_ACTION, "$n is ranting on about 'the answer', 'ONE' and other mumbo-jumbo...", ch, NULL, NULL, TO_ROOM );
            break;
          case 10:
            send_to_char( "&rYou feel the end is near.\n\r", ch );
            act( AT_ACTION, "$n is muttering and ranting in tongues...", ch, NULL, NULL, TO_ROOM );
            break;
        }
      if( ch->mental_state <= -30 )
        switch ( ( abs( ch->mental_state ) + 5 ) / 10 )
        {
          case 10:
            if( ch->position > POS_SLEEPING )
            {
              if( ( ch->position == POS_STANDING
                    || ch->position < POS_FIGHTING ) && number_percent(  ) + 10 < abs( ch->mental_state ) )
                do_sleep( ch, "" );
              else
                send_to_char( "You're barely conscious.\n\r", ch );
            }
            break;
          case 9:
            if( ch->position > POS_SLEEPING )
            {
              if( ( ch->position == POS_STANDING
                    || ch->position < POS_FIGHTING ) && ( number_percent(  ) + 20 ) < abs( ch->mental_state ) )
                do_sleep( ch, "" );
              else
                send_to_char( "You can barely keep your eyes open.\n\r", ch );
            }
            break;
          case 8:
            if( ch->position > POS_SLEEPING )
            {
              if( ch->position < POS_SITTING && ( number_percent(  ) + 30 ) < abs( ch->mental_state ) )
                do_sleep( ch, "" );
              else
                send_to_char( "You're extremely drowsy.\n\r", ch );
            }
            break;
          case 7:
            if( ch->position > POS_RESTING )
              send_to_char( "You feel very unmotivated.\n\r", ch );
            break;
          case 6:
            if( ch->position > POS_RESTING )
              send_to_char( "You feel sedated.\n\r", ch );
            break;
          case 5:
            if( ch->position > POS_RESTING )
              send_to_char( "You feel sleepy.\n\r", ch );
            break;
          case 4:
            if( ch->position > POS_RESTING )
              send_to_char( "You feel tired.\n\r", ch );
            break;
          case 3:
            if( ch->position > POS_RESTING )
              send_to_char( "You could use a rest.\n\r", ch );
            break;
        }
      if( ch->timer > 24 )
        do_quit( ch, "" );
      else if( ch == ch_save && IS_SET( sysdata.save_flags, SV_AUTO ) && ++save_count < 10 )  /* save max of 10 per tick */
        save_char_obj( ch );
    }
  }

  return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{
  OBJ_DATA *obj;
  sh_int AT_TEMP;

  for( obj = last_object; obj; obj = gobj_prev )
  {
    CHAR_DATA *rch;
    char *message;

    if( obj == first_object && obj->prev )
    {
      bug( "obj_update: first_object->prev != NULL... fixed", 0 );
      obj->prev = NULL;
    }
    gobj_prev = obj->prev;
    if( gobj_prev && gobj_prev->next != obj )
    {
      bug( "obj_update: obj->prev->next != obj", 0 );
      return;
    }
    set_cur_obj( obj );
    if( obj->carried_by )
      oprog_random_trigger( obj );
    else if( obj->in_room && obj->in_room->area->nplayer > 0 )
      oprog_random_trigger( obj );

    if( obj_extracted( obj ) )
      continue;

    if( obj->item_type == ITEM_PIPE )
    {
      if( IS_SET( obj->value[3], PIPE_LIT ) )
      {
        if( --obj->value[1] <= 0 )
        {
          obj->value[1] = 0;
          REMOVE_BIT( obj->value[3], PIPE_LIT );
        }
        else if( IS_SET( obj->value[3], PIPE_HOT ) )
          REMOVE_BIT( obj->value[3], PIPE_HOT );
        else
        {
          if( IS_SET( obj->value[3], PIPE_GOINGOUT ) )
          {
            REMOVE_BIT( obj->value[3], PIPE_LIT );
            REMOVE_BIT( obj->value[3], PIPE_GOINGOUT );
          }
          else
            SET_BIT( obj->value[3], PIPE_GOINGOUT );
        }
        if( !IS_SET( obj->value[3], PIPE_LIT ) )
          SET_BIT( obj->value[3], PIPE_FULLOFASH );
      }
      else
        REMOVE_BIT( obj->value[3], PIPE_HOT );
    }


/* Corpse decay (npc corpses decay at 8 times the rate of pc corpses) - Narn */

    if( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
    {
      sh_int timerfrac = UMAX( 1, obj->timer - 1 );
      if( obj->item_type == ITEM_CORPSE_PC )
        timerfrac = ( int )( obj->timer / 2 + 1 );

      if( obj->timer > 0 && obj->value[2] > timerfrac )
      {
        char buf[MAX_STRING_LENGTH];
        char name[MAX_STRING_LENGTH];
        char *bufptr;
        bufptr = one_argument( obj->short_descr, name );
        bufptr = one_argument( bufptr, name );
        bufptr = one_argument( bufptr, name );

        separate_obj( obj );
        obj->value[2] = timerfrac;
        sprintf( buf, corpse_descs[UMIN( timerfrac - 1, 4 )], bufptr );

        STRFREE( obj->description );
        obj->description = STRALLOC( buf );
      }
    }

    /*
     * don't let inventory decay 
     */
    if( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
      continue;

    /*
     * groundrot items only decay on the ground 
     */
    if( IS_OBJ_STAT( obj, ITEM_GROUNDROT ) && !obj->in_room )
      continue;


    if( ( obj->timer <= 0 || --obj->timer > 0 ) )
      continue;

    /*
     * if we get this far, object's timer has expired. 
     */

    AT_TEMP = AT_PLAIN;
    switch ( obj->item_type )
    {
      default:
        message = "$p mysteriously vanishes.";
        AT_TEMP = AT_PLAIN;
        break;
      case ITEM_CONTAINER:
        message = "$p falls apart, tattered from age.";
        AT_TEMP = AT_OBJECT;
        break;
      case ITEM_PORTAL:
        message = "$p unravels and winks from existence.";
        remove_portal( obj );
        obj->item_type = ITEM_TRASH;  /* so extract_obj  */
        AT_TEMP = AT_MAGIC; /* doesn't remove_portal */
        break;
      case ITEM_FOUNTAIN:
        message = "$p dries up.";
        AT_TEMP = AT_BLUE;
        break;
      case ITEM_CORPSE_NPC:
      case ITEM_TRASH:
        message = "$p decays into dust and blows away.";
        AT_TEMP = AT_OBJECT;
        break;
      case ITEM_CORPSE_PC:
        message = "$p is sucked into a swirling vortex of colors...";
        AT_TEMP = AT_MAGIC;
        break;
      case ITEM_COOK:
      case ITEM_FOOD:
        message = "$p is devoured by a swarm of maggots.";
        AT_TEMP = AT_HUNGRY;
        break;
      case ITEM_BLOOD:
        message = "$p slowly seeps into the ground.";
        AT_TEMP = AT_BLOOD;
        break;
      case ITEM_BLOODSTAIN:
        message = "$p dries up into flakes and blows away.";
        AT_TEMP = AT_BLOOD;
        break;
      case ITEM_SCRAPS:
        message = "$p crumble and decay into nothing.";
        AT_TEMP = AT_OBJECT;
        break;
      case ITEM_FIRE:
        /*
         * This is removed because it is done in obj_from_room
         * * Thanks to gfinello@mail.karmanet.it for pointing this out.
         * * --Shaddai
         * if (obj->in_room)
         * {
         * --obj->in_room->light;
         * if ( obj->in_room->light < 0 )
         * obj->in_room->light = 0;
         * }
         */
        message = "$p burns out.";
        AT_TEMP = AT_FIRE;
    }

    if( obj->carried_by )
    {
      act( AT_TEMP, message, obj->carried_by, obj, NULL, TO_CHAR );
    }
    else if( obj->in_room && ( rch = obj->in_room->first_person ) != NULL && !IS_OBJ_STAT( obj, ITEM_BURIED ) )
    {
      act( AT_TEMP, message, rch, obj, NULL, TO_ROOM );
      act( AT_TEMP, message, rch, obj, NULL, TO_CHAR );
    }

    if( obj->serial == cur_obj )
      global_objcode = rOBJ_EXPIRED;
    extract_obj( obj );
  }
  return;
}


/*
 * Function to check important stuff happening to a player
 * This function should take about 5% of mud cpu time
 */
void char_check( void )
{
  CHAR_DATA *ch, *ch_next;
  OBJ_DATA *obj;
  EXIT_DATA *pexit = NULL;
  static int cnt = 0;
  int door, retcode;

  /*
   * This little counter can be used to handle periodic events 
   */
  cnt = ( cnt + 1 ) % SECONDS_PER_TICK;

  for( ch = first_char; ch; ch = ch_next )
  {
    set_cur_char( ch );
    ch_next = ch->next;
    will_fall( ch, 0 );

    if( char_died( ch ) )
      continue;

    if( IS_NPC( ch ) )
    {
      if( ( cnt & 1 ) )
        continue;

      /*
       * running mobs -Thoric 
       */
      if( xIS_SET( ch->act, ACT_RUNNING ) )
      {
        if( !xIS_SET( ch->act, ACT_SENTINEL ) && !ch->fighting && ch->hunting )
        {
          WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
          hunt_victim( ch );
          continue;
        }

        if( ch->spec_fun )
        {
          if( ( *ch->spec_fun ) ( ch ) )
            continue;
          if( char_died( ch ) )
            continue;
        }

        if( !xIS_SET( ch->act, ACT_SENTINEL )
            && !xIS_SET( ch->act, ACT_PROTOTYPE )
            && ( door = number_bits( 4 ) ) <= 9
            && ( pexit = get_exit( ch->in_room, door ) ) != NULL
            && pexit->to_room
            && !IS_SET( pexit->exit_info, EX_CLOSED )
            && !xIS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
            && !xIS_SET( pexit->to_room->room_flags, ROOM_DEATH )
            && ( !xIS_SET( ch->act, ACT_STAY_AREA ) || pexit->to_room->area == ch->in_room->area ) )
        {
          retcode = move_char( ch, pexit, 0 );
          if( char_died( ch ) )
            continue;
          if( retcode != rNONE || xIS_SET( ch->act, ACT_SENTINEL ) || ch->position < POS_STANDING )
            continue;
        }
      }
      continue;
    }
    else
    {
      if( ch->mount && ch->in_room != ch->mount->in_room )
      {
        xREMOVE_BIT( ch->mount->act, ACT_MOUNTED );
        ch->mount = NULL;
        ch->position = POS_STANDING;
        send_to_char( "No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", ch );
      }

      if( ( ch->in_room && ch->in_room->sector_type == SECT_UNDERWATER )
          || ( ch->in_room && ch->in_room->sector_type == SECT_OCEANFLOOR ) )
      {
        if( !IS_AFFECTED( ch, AFF_AQUA_BREATH ) )
        {
          if( ch->level < LEVEL_IMMORTAL
              && !IS_ICER( ch )
              && !is_android( ch ) && !is_superandroid( ch ) && !wearing_sentient_chip( ch ) && !is_bio( ch ) )
          {
            int dam;

            /*
             * Changed level of damage at Brittany's request. -- Narn 
             */
            dam = number_range( ch->max_hit / 100, ch->max_hit / 50 );
            dam = UMAX( 1, dam );
            if( number_bits( 3 ) == 0 )
              send_to_char( "You cough and choke as you try to breathe water!\n\r", ch );
            damage( ch, ch, dam, TYPE_UNDEFINED );
          }
        }
      }

      if( ( ch->in_room && ch->in_room->sector_type == SECT_SPACE ) )
      {
        if( !IS_ICER( ch )
            && !is_android( ch )
            && !is_reploid( ch )
            && !is_bio( ch ) && !is_superandroid( ch ) && !wearing_sentient_chip( ch ) && !wearing_space_suit( ch ) )
        {
          if( get_trust( ch ) < LEVEL_IMMORTAL )
          {
            int dam;

            dam = number_range( ch->max_hit / 100, ch->max_hit / 50 );
            dam = UMAX( 1, dam );
            if( number_bits( 3 ) == 0 )
            {
              send_to_char( "You gasp for air but find none!\n\r", ch );
            }
            damage( ch, ch, dam, TYPE_UNDEFINED );
          }
        }
      }

      if( char_died( ch ) )
        continue;

      if( ch->in_room
          && ( ( ch->in_room->sector_type == SECT_WATER_NOSWIM ) || ( ch->in_room->sector_type == SECT_WATER_SWIM ) ) )
      {
        if( !IS_AFFECTED( ch, AFF_FLYING )
            && !IS_AFFECTED( ch, AFF_FLOATING ) && !IS_AFFECTED( ch, AFF_AQUA_BREATH ) && !ch->mount )
        {
          for( obj = ch->first_carrying; obj; obj = obj->next_content )
            if( obj->item_type == ITEM_BOAT )
              break;

/*
		    if ( !obj )
		    {
			if ( ch->level < LEVEL_IMMORTAL )
			{
			    int mov;
			    int dam;

			    if ( ch->move > 0 )
			    {
				mov = number_range( ch->max_move / 20, ch->max_move / 5 );
				mov = UMAX( 1, mov );

				if ( ch->move - mov < 0 )
				    ch->move = 0;
				else
				    ch->move -= mov;
			    }
			    else
			    {
				dam = number_range( ch->max_hit / 20, ch->max_hit / 5 );
				dam = UMAX( 1, dam );

				if ( number_bits(3) == 0 )
				   send_to_char( "Struggling with exhaustion, you choke on a mouthful of water.\n\r", ch );
				damage( ch, ch, dam, TYPE_UNDEFINED );
			    }
			}
		    }
*/
        }
      }

      /*
       * beat up on link dead players 
       */
      if( !ch->desc )
      {
        CHAR_DATA *wch, *wch_next;

        for( wch = ch->in_room->first_person; wch; wch = wch_next )
        {
          wch_next = wch->next_in_room;

          if( !IS_NPC( wch )
              || wch->fighting
              || IS_AFFECTED( wch, AFF_CHARM )
              || !IS_AWAKE( wch ) || ( xIS_SET( wch->act, ACT_WIMPY ) && IS_AWAKE( ch ) ) || !can_see( wch, ch ) )
            continue;

          if( is_hating( wch, ch ) )
          {
            found_prey( wch, ch );
            continue;
          }

          if( ( !xIS_SET( wch->act, ACT_AGGRESSIVE )
                && !xIS_SET( wch->act, ACT_META_AGGR ) )
              || xIS_SET( wch->act, ACT_MOUNTED ) || xIS_SET( wch->in_room->room_flags, ROOM_SAFE ) )
            continue;
          ch->fight_start = ch->exp;
          global_retcode = multi_hit( wch, ch, TYPE_UNDEFINED );
        }
      }
    }
  }
}

/*
 * Aggress.
 *
 * for each descriptor
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function should take 5% to 10% of ALL mud cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 */
void aggr_update( void )
{
  DESCRIPTOR_DATA *d, *dnext;
  CHAR_DATA *wch;
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *victim;
  struct act_prog_data *apdtmp;

#ifdef UNDEFD
  /*
   *  GRUNT!  To do
   *
   */
  if( IS_NPC( wch ) && wch->mpactnum > 0 && wch->in_room->area->nplayer > 0 )
  {
    MPROG_ACT_LIST *tmp_act, *tmp2_act;
    for( tmp_act = wch->mpact; tmp_act; tmp_act = tmp_act->next )
    {
      oprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch, tmp_act->obj, tmp_act->vo, ACT_PROG );
      DISPOSE( tmp_act->buf );
    }
    for( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
    {
      tmp2_act = tmp_act->next;
      DISPOSE( tmp_act );
    }
    wch->mpactnum = 0;
    wch->mpact = NULL;
  }
#endif

  /*
   * check mobprog act queue 
   */
  while( ( apdtmp = mob_act_list ) != NULL )
  {
    wch = mob_act_list->vo;
    if( !char_died( wch ) && wch->mpactnum > 0 )
    {
      MPROG_ACT_LIST *tmp_act;

      while( ( tmp_act = wch->mpact ) != NULL )
      {
        if( tmp_act->obj && obj_extracted( tmp_act->obj ) )
          tmp_act->obj = NULL;
        if( tmp_act->ch && !char_died( tmp_act->ch ) )
          mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch, tmp_act->obj, tmp_act->vo, ACT_PROG );
        wch->mpact = tmp_act->next;
        DISPOSE( tmp_act->buf );
        DISPOSE( tmp_act );
      }
      wch->mpactnum = 0;
      wch->mpact = NULL;
    }
    mob_act_list = apdtmp->next;
    DISPOSE( apdtmp );
  }


  /*
   * Just check descriptors here for victims to aggressive mobs
   * We can check for linkdead victims in char_check  -Thoric
   */
  for( d = first_descriptor; d; d = dnext )
  {
    dnext = d->next;
    if( d->connected != CON_PLAYING || ( wch = d->character ) == NULL )
      continue;

    if( char_died( wch ) || IS_NPC( wch ) || wch->level >= LEVEL_IMMORTAL || !wch->in_room )
      continue;

    for( ch = wch->in_room->first_person; ch; ch = ch_next )
    {
      int count;

      ch_next = ch->next_in_room;

      if( !IS_NPC( ch )
          || ch->fighting
          || IS_AFFECTED( ch, AFF_CHARM )
          || !IS_AWAKE( ch ) || ( xIS_SET( ch->act, ACT_WIMPY ) && IS_AWAKE( wch ) ) || !can_see( ch, wch ) )
        continue;

      if( is_hating( ch, wch ) )
      {
        found_prey( ch, wch );
        continue;
      }

      if( ( !xIS_SET( ch->act, ACT_AGGRESSIVE )
            && !xIS_SET( ch->act, ACT_META_AGGR ) )
          || xIS_SET( ch->act, ACT_MOUNTED ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
        continue;

      /*
       * Ok we have a 'wch' player character and a 'ch' npc aggressor.
       * Now make the aggressor fight a RANDOM pc victim in the room,
       *   giving each 'vch' an equal chance of selection.
       *
       * Depending on flags set, the mob may attack another mob
       */
      count = 0;
      victim = NULL;
      for( vch = wch->in_room->first_person; vch; vch = vch_next )
      {
        vch_next = vch->next_in_room;

        if( ( !IS_NPC( vch ) || xIS_SET( ch->act, ACT_META_AGGR )
              || xIS_SET( vch->act, ACT_ANNOYING ) )
            && vch->level < LEVEL_IMMORTAL && ( !xIS_SET( ch->act, ACT_WIMPY ) || !IS_AWAKE( vch ) ) && can_see( ch, vch ) )
        {
          if( number_range( 0, count ) == 0 )
            victim = vch;
          count++;
        }
      }

      if( !victim )
      {
        bug( "Aggr_update: null victim.", count );
        continue;
      }

      if( get_timer( victim, TIMER_PKILLED ) > 0 )
        continue;

      /*
       * backstabbing mobs (Thoric) 
       */
      if( IS_NPC( ch ) && xIS_SET( ch->attacks, ATCK_BACKSTAB ) )
      {
        OBJ_DATA *obj;

        if( !ch->mount
            && ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL
            && ( obj->value[3] == 11 || obj->value[3] == 2 ) && !victim->fighting && victim->hit >= victim->max_hit )
        {
          check_attacker( ch, victim );
          WAIT_STATE( ch, skill_table[gsn_backstab]->beats );
          if( !IS_AWAKE( victim ) || number_percent(  ) + 5 < ch->level )
          {
            victim->fight_start = victim->exp;
            global_retcode = multi_hit( ch, victim, gsn_backstab );
            continue;
          }
          else
          {
            victim->fight_start = victim->exp;
            global_retcode = damage( ch, victim, 0, gsn_backstab );
            continue;
          }
        }
      }
      victim->fight_start = victim->exp;
      global_retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
    }
  }

  return;
}

/* From interp.c */
bool check_social args( ( CHAR_DATA * ch, char *command, char *argument ) );

/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void drunk_randoms( CHAR_DATA * ch )
{
  CHAR_DATA *rvch = NULL;
  CHAR_DATA *vch;
  sh_int drunk;
  sh_int position;

  if( IS_NPC( ch ) || ch->pcdata->condition[COND_DRUNK] <= 0 )
    return;

  if( number_percent(  ) < 30 )
    return;

  drunk = ch->pcdata->condition[COND_DRUNK];
  position = ch->position;
  ch->position = POS_STANDING;

  if( number_percent(  ) < ( 2 * drunk / 20 ) )
    check_social( ch, "burp", "" );
  else if( number_percent(  ) < ( 2 * drunk / 20 ) )
    check_social( ch, "hiccup", "" );
  else if( number_percent(  ) < ( 2 * drunk / 20 ) )
    check_social( ch, "drool", "" );
  else if( number_percent(  ) < ( 2 * drunk / 20 ) )
    check_social( ch, "fart", "" );
  else if( drunk > ( 10 + ( get_curr_con( ch ) / 5 ) ) && number_percent(  ) < ( 2 * drunk / 18 ) )
  {
    for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
      if( number_percent(  ) < 10 )
        rvch = vch;
    check_social( ch, "puke", ( rvch ? rvch->name : "" ) );
  }

  ch->position = position;
  return;
}

/*
 * Random hallucinations for those suffering from an overly high mentalstate
 * (Hats off to Albert Hoffman's "problem child")	-Thoric
 */
void hallucinations( CHAR_DATA * ch )
{
  if( ch->mental_state >= 30 && number_bits( 5 - ( ch->mental_state >= 50 ) - ( ch->mental_state >= 75 ) ) == 0 )
  {
    char *t;

    switch ( number_range( 1, UMIN( 21, ( ch->mental_state + 5 ) / 5 ) ) )
    {
      default:
      case 1:
        t = "&gYou feel very restless... you can't sit still.\n\r";
        break;
      case 2:
        t = "&gYou're tingling all over.\n\r";
        break;
        //case  3: t = "Your skin is crawling.\n\r";break;
      case 3:
        t = "&rYou're running around CoU while pkred at 1 pl trying to escape Sigma.\n\r";
        break;
      case 4:
        t = "&rYou suddenly feel that something is terribly wrong.\n\r";
        break;
      case 5:
        t = "&RThose damn little fairies keep laughing at you!\n\r";
        break;
      case 6:
        t = "&gYou can hear your mother crying...\n\r";
        break;
      case 7:
        t = "&cHave you been here before, or not?  You're not sure...\n\r";
        break;
      case 8:
        t = "&rPainful childhood memories flash through your mind.\n\r";
        break;
      case 9:
        t = "&cYou hear someone call your name in the distance...\n\r";
        break;
      case 10:
        t = "&gYour head is pulsating... you can't think straight.\n\r";
        break;
        //case 11: t = "The ground... seems to be squirming...\n\r";break;
      case 11:
        t = "&rSigma walks up to you and takes all your items and zeni.\n\r";
        break;
      case 12:
        t = "&gYou're not quite sure what is real anymore.\n\r";
        break;
      case 13:
        t = "&gIt's all a dream... or is it?\n\r";
        break;
      case 14:
        t = "&gYou hear your grandchildren praying for you to watch over them.\n\r";
        break;
      case 15:
        t = "&RThey're coming to get you... coming to take you away...\n\r";
        break;
      case 16:
        t = "&cYou begin to feel all powerful!\n\r";
        break;
      case 17:
        t = "&gYou're light as air... the heavens are yours for the taking.\n\r";
        break;
      case 18:
        t = "&gYour whole life flashes by... and your future...\n\r";
        break;
      case 19:
        t = "&gYou are everywhere and everything... you know all and are all!\n\r";
        break;
      case 20:
        t = "&gYou feel immortal!\n\r";
        break;
      case 21:
        t = "&gAhh... the power of a Supreme Entity... what to do...\n\r";
        break;
    }
    ch_printf( ch, "&r%s", t );
    //send_to_char( t, ch );
  }
  return;
}

void tele_update( void )
{
  TELEPORT_DATA *tele, *tele_next;

  if( !first_teleport )
    return;

  for( tele = first_teleport; tele; tele = tele_next )
  {
    tele_next = tele->next;
    if( --tele->timer <= 0 )
    {
      if( tele->room->first_person )
      {
        if( xIS_SET( tele->room->room_flags, ROOM_TELESHOWDESC ) )
          teleport( tele->room->first_person, tele->room->tele_vnum, TELE_SHOWDESC | TELE_TRANSALL );
        else
          teleport( tele->room->first_person, tele->room->tele_vnum, TELE_TRANSALL );
      }
      UNLINK( tele, first_teleport, last_teleport, next, prev );
      DISPOSE( tele );
    }
  }
}
bool admin_online( void )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *vch;

  for( d = first_descriptor; d; d = d->next )
  {
    vch = d->character;

    if( IS_IMMORTAL( vch ) )
    {
      return TRUE;
    }
  }

  return FALSE;
}

void save_bandwidthLog(  )
{
  FILE *fp;

  if( ( fp = fopen( BANDWIDTH_LOG_FILE, "a" ) ) == NULL )
  {
    bug( "Could not open %s file", BANDWIDTH_LOG_FILE );
    return;
  }

  fprintf( fp, "%d,", sysdata.outBytesOther );
  fprintf( fp, "%d,", sysdata.outBytesChannel );
  fprintf( fp, "%d,", sysdata.outBytesCombat );
  fprintf( fp, "%d,", sysdata.outBytesMovement );
  fprintf( fp, "%d,", sysdata.outBytesInformation );
  fprintf( fp, "%d,", sysdata.outBytesPrompt );
  fprintf( fp, "%d,", sysdata.outBytesInfoChannel );
  fprintf( fp, "%s", ctime( &current_time ) );
//  fprintf( fp, "\n" );
  sysdata.outBytesOther = 0;
  sysdata.outBytesChannel = 0;
  sysdata.outBytesCombat = 0;
  sysdata.outBytesMovement = 0;
  sysdata.outBytesInformation = 0;
  sysdata.outBytesPrompt = 0;
  sysdata.outBytesInfoChannel = 0;

  fclose( fp );

  return;
}

extern char *const clan_alliance_type[];


void update_alliances( void )
{
  ALLIANCE_DATA *alliance;
  ALLIANCE_DATA *valliance;
  char buf[MAX_STRING_LENGTH];
  bool changed = FALSE;

  for( alliance = first_alliance; alliance; alliance = alliance->next )
  {
    if( alliance->changeTimer <= 0 )
      continue;

    changed = TRUE;
    alliance->changeTimer--;

    if( alliance->changeTimer <= 0 )
    {
      alliance->status = alliance->newStatus;
      valliance = get_alliance( alliance->vclan, alliance->clan );
      valliance->vclanStatus = alliance->newStatus;
      alliance->changeTimer = 0;
      alliance->newStatus = 0;
      sprintf( buf, "%s has changed their alliance with your clan to %s", alliance->clan->name,
               clan_alliance_type[alliance->status] );
      echo_to_clan( alliance->vclan, buf );
      sprintf( buf, "Your clan has changed their alliance with %s to %s.", alliance->vclan->name,
               clan_alliance_type[alliance->status] );
      echo_to_clan( alliance->clan, buf );
      if( allianceStatus( alliance->clan, alliance->vclan ) == ALLIANCE_ALLIED )
      {
        sprintf( buf, "&C%s and %s are now allies.\n\r", alliance->clan->name, alliance->vclan->name );
        echo_to_clan( alliance->clan, buf );
        echo_to_clan( alliance->vclan, buf );
      }
      if( allianceStatus( alliance->clan, alliance->vclan ) == ALLIANCE_ATWAR )
      {
        sprintf( buf, "%s and %s have begun a massive clan war!\n\r", alliance->clan->name, alliance->vclan->name );
        echo_to_all( AT_RED, buf, ECHOTAR_ALL );
      }
    }
  }
  if( changed )
    save_alliance(  );
  return;
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
  static int pulse_area;
  static int pulse_mobile;
  static int pulse_violence;
  static int pulse_point;
  static int pulse_second;
  static int pulse_houseauc;
  static int pulse_space;
  static int pulse_ship;
  static int pulse_summon;
  static int pulse_recharge;
  static int pulse_minute;
  static int pulse_saveBandwidthLog;
  struct timeval stime;
  struct timeval etime;

  if( timechar )
  {
    set_char_color( AT_PLAIN, timechar );
    send_to_char( "Starting update timer.\n\r", timechar );
    gettimeofday( &stime, NULL );
  }

  if( --pulse_saveBandwidthLog <= 0 )
  {
    pulse_saveBandwidthLog = 5 * PULSE_MINUTE;
    save_bandwidthLog(  );
  }

  if( --pulse_houseauc <= 0 )
  {
    pulse_houseauc = 1800 * PULSE_PER_SECOND;
    homebuy_update(  );
  }

  if( --pulse_area <= 0 )
  {
    pulse_area = number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
    area_update(  );
  }

  if( --pulse_mobile <= 0 )
  {
    pulse_mobile = PULSE_MOBILE;
    mobile_update(  );
  }

  if( --pulse_summon <= 0 )
  {
    pulse_summon = PULSE_SUMMON;
    summon_update(  );
  }

  if( --pulse_space <= 0 )
  {
    pulse_space = PULSE_SPACE;
    update_space(  );
    update_bus(  );
    update_traffic(  );
  }

  if( --pulse_recharge <= 0 )
  {
    pulse_recharge = PULSE_SPACE / 3;
    recharge_ships(  );
  }

  if( --pulse_ship <= 0 )
  {
    pulse_ship = PULSE_SPACE / 10;
    move_ships(  );
  }

  if( --pulse_violence <= 0 )
  {
    pulse_violence = PULSE_VIOLENCE;
    violence_update(  );
  }

  if( --pulse_point <= 0 )
  {
    pulse_point = number_range( PULSE_TICK * 0.71, PULSE_TICK );

    auth_update(  );  /* Gorog */
    time_update(  );
    weather_update(  );
    char_update(  );
    obj_update(  );
    clear_vrooms(  ); /* remove virtual rooms */
    update_alliances(  );
    rank_update(  );
  }

  if( --pulse_minute <= 0 )
  {
    pulse_minute = PULSE_MINUTE;
    if( sysdata.kaiRestoreTimer > 0 )
      sysdata.kaiRestoreTimer -= 1;
    mystic_check(  );
    save_economy(  );
  }

  if( --pulse_second <= 0 )
  {
    pulse_second = PULSE_PER_SECOND;
    char_check(  );
    check_dns(  );
    check_pfiles( 0 );
    reboot_check( 0 );
    mana_check(  );
  }

  if( --auction->pulse <= 0 )
  {
    auction->pulse = PULSE_AUCTION;
    auction_update(  );
    dball_check(  );
  }

  mpsleep_update(  ); /* Check for sleeping mud progs -rkb */

  tele_update(  );
  aggr_update(  );
  obj_act_update(  );
  room_act_update(  );
  clean_obj_queue(  );  /* dispose of extracted objects */
  clean_char_queue(  ); /* dispose of dead mobs/quitting chars */
  if( timechar )
  {
    gettimeofday( &etime, NULL );
    set_char_color( AT_PLAIN, timechar );
    send_to_char( "Update timing complete.\n\r", timechar );
    subtract_times( &etime, &stime );
    ch_printf( timechar, "Timing took %d.%06d seconds.\n\r", etime.tv_sec, etime.tv_usec );
    timechar = NULL;
  }
  tail_chain(  );
  return;
}


void remove_portal( OBJ_DATA * portal )
{
  ROOM_INDEX_DATA *fromRoom, *toRoom;
  EXIT_DATA *pexit;
  bool found;

  if( !portal )
  {
    bug( "remove_portal: portal is NULL", 0 );
    return;
  }

  fromRoom = portal->in_room;
  found = FALSE;
  if( !fromRoom )
  {
    bug( "remove_portal: portal->in_room is NULL", 0 );
    return;
  }

  for( pexit = fromRoom->first_exit; pexit; pexit = pexit->next )
    if( IS_SET( pexit->exit_info, EX_PORTAL ) )
    {
      found = TRUE;
      break;
    }

  if( !found )
  {
    bug( "remove_portal: portal not found in room %d!", fromRoom->vnum );
    return;
  }

  if( pexit->vdir != DIR_PORTAL )
    bug( "remove_portal: exit in dir %d != DIR_PORTAL", pexit->vdir );

  if( ( toRoom = pexit->to_room ) == NULL )
    bug( "remove_portal: toRoom is NULL", 0 );

  extract_exit( fromRoom, pexit );

  return;
}

void reboot_check( time_t reset )
{
  static char *tmsg[] = { "You feel the ground shake as the end comes near!",
    "Lightning crackles in the sky above!",
    "Crashes of thunder sound across the land!",
    "The sky has suddenly turned midnight black.",
    "You notice the life forms around you slowly dwindling away.",
    "The seas across the realm have turned frigid.",
    "The life forces throughout the universe seem slightly unstable.",
    "You sense a change in the life forces surrounding you."
  };
  static const int times[] = { 60, 120, 180, 240, 300, 600, 900, 1800 };
  static const int timesize = UMIN( sizeof( times ) / sizeof( *times ), sizeof( tmsg ) / sizeof( *tmsg ) );
  char buf[MAX_STRING_LENGTH];
  static int trun;
  static bool init = FALSE;

  if( !init || reset >= current_time )
  {
    for( trun = timesize - 1; trun >= 0; trun-- )
      if( reset >= current_time + times[trun] )
        break;
    init = TRUE;
    return;
  }

  if( ( current_time % 1800 ) == 0 )
  {
    sprintf( buf, "%.24s: %d players", ctime( &current_time ), num_descriptors );
    append_to_file( USAGE_FILE, buf );
    sprintf( buf, "%.24s:  %dptn  %dpll  %dsc %dbr  %d global loot",
             ctime( &current_time ),
             sysdata.upotion_val, sysdata.upill_val, sysdata.scribed_used, sysdata.brewed_used, sysdata.global_looted );
    append_to_file( ECONOMY_FILE, buf );
  }

/*
  if ( new_boot_time_t - boot_time < 60*60*18 &&
      !set_boot_time->manual )
    return;
*/

  if( new_boot_time_t <= current_time )
  {
    CHAR_DATA *vch;
    extern bool mud_down;

    if( auction->item )
    {
      sprintf( buf, "Sale of %s has been stopped by mud.", auction->item->short_descr );
      talk_auction( buf );
      obj_to_char( auction->item, auction->seller );
      auction->item = NULL;
      if( auction->buyer && auction->buyer != auction->seller )
      {
        auction->buyer->gold += auction->bet;
        send_to_char( "Your money has been returned.\n\r", auction->buyer );
      }
    }
    echo_to_all( AT_YELLOW, "You are forced from the universe by a strong "
                 "presence\n\ras life here is reconstructed.", ECHOTAR_ALL );
    save_classes(  );
    save_commands(  );
    save_socials(  );
    save_skill_table(  );
    save_herb_table(  );
    log_string( "Automatic Reboot" );
    for( vch = first_char; vch; vch = vch->next )
      if( !IS_NPC( vch ) )
        save_char_obj( vch );
    mud_down = TRUE;
    return;
  }

  if( trun != -1 && new_boot_time_t - current_time <= times[trun] )
  {
    echo_to_all( AT_YELLOW, tmsg[trun], ECHOTAR_ALL );
//    echo_to_all(AT_YELLOW, leetmsg[trun], ECHOTAR_ALL);
    if( trun <= 5 )
      sysdata.DENY_NEW_PLAYERS = TRUE;
    --trun;
    return;
  }
  return;
}

void bump_que(  )
{
  int i = 0;

  if( auction->queued_obj[0] == NULL )
  {
    auction->freeQueSlot = -1;
    auction->queued_obj[0] = NULL;
    auction->queued_char[0] = NULL;
    auction->queued_starting_bid[0] = 0;
    auction->queued_obj[1] = NULL;
    auction->queued_char[1] = NULL;
    auction->queued_starting_bid[1] = 0;
    auction->queued_obj[2] = NULL;
    auction->queued_char[2] = NULL;
    auction->queued_starting_bid[2] = 0;
    auction->queued_obj[3] = NULL;
    auction->queued_char[3] = NULL;
    auction->queued_starting_bid[3] = 0;
    auction->queued_obj[4] = NULL;
    auction->queued_char[4] = NULL;
    auction->queued_starting_bid[4] = 0;
    return;
  }

  for( i = 0; i < AUCTION_QUE; i++ )
  {
    auction->queued_obj[i] = auction->queued_obj[i + 1];
    auction->queued_char[i] = auction->queued_char[i + 1];
    auction->queued_starting_bid[i] = auction->queued_starting_bid[i + 1];
  }
  auction->queued_obj[3] = auction->queued_obj[4];
  auction->queued_char[3] = auction->queued_char[4];
  auction->queued_starting_bid[3] = auction->queued_starting_bid[4];
  auction->queued_obj[4] = NULL;
  auction->queued_char[4] = NULL;
  auction->queued_starting_bid[4] = 0;

  for( i = 0; i <= AUCTION_QUE; i++ )
  {
    if( i == 0 && auction->queued_obj[0] == NULL )
    {
      auction->freeQueSlot = -1;
      continue;
    }

    if( auction->queued_obj[i] == NULL )
    {
      auction->freeQueSlot = i;
      continue;
    }
  }

  return;
}

/* the auction update*/

void auction_update( void )
{
  int tax, pay;
  char buf[MAX_STRING_LENGTH];

  if( !auction->item )
  {
    if( AUCTION_MEM > 0 && auction->history[0] && ++auction->hist_timer == 6 * AUCTION_MEM )
    {
      int i;

      for( i = AUCTION_MEM - 1; i >= 0; i-- )
      {
        if( auction->history[i] )
        {
          auction->history[i] = NULL;
          auction->hist_timer = 0;
          break;
        }
      }
    }
    return;
  }

  sysdata.outBytesFlag = LOGBOUTCHANNEL;

  switch ( ++auction->going ) /* increase the going state */
  {
    case 1:  /* going once */
    case 2:  /* going twice */
      if( auction->bet > auction->starting )
        sprintf( buf, "&C%s&c: going &C%s &cfor &C%s&C.", auction->item->short_descr,
                 ( ( auction->going == 1 ) ? "once" : "twice" ), num_punct( auction->bet ) );
      else
        sprintf( buf, "&C%s&c: going &C%s&c (bid not received yet).", auction->item->short_descr,
                 ( ( auction->going == 1 ) ? "once" : "twice" ) );

      talk_auction( buf );
      break;

    case 3:  /* SOLD! */
      if( !auction->buyer && auction->bet )
      {
        bug( "Auction code reached SOLD, with NULL buyer, but %d zeni bid", auction->bet );
        auction->bet = 0;
      }
      if( auction->bet > 0 && auction->buyer != auction->seller )
      {
        sprintf( buf, "&C%s &csold to &C%s &cfor &C%s&c.",
                 auction->item->short_descr,
                 IS_NPC( auction->buyer ) ? auction->buyer->short_descr : auction->buyer->name, num_punct( auction->bet ) );
        talk_auction( buf );

        act( AT_ACTION, "The auctioneer materializes before you, and hands you $p.",
             auction->buyer, auction->item, NULL, TO_CHAR );
        act( AT_ACTION, "The auctioneer materializes before $n, and hands $m $p.",
             auction->buyer, auction->item, NULL, TO_ROOM );

        if( ( auction->buyer->carry_weight + get_obj_weight( auction->item ) ) > can_carry_w( auction->buyer ) )
        {
          act( AT_PLAIN, "$p is too heavy for you to carry with your current inventory.", auction->buyer, auction->item,
               NULL, TO_CHAR );
          act( AT_PLAIN, "$n is carrying too much to also carry $p, and $e drops it.", auction->buyer, auction->item, NULL,
               TO_ROOM );
          obj_to_room( auction->item, auction->buyer->in_room );
        }
        else
          obj_to_char( auction->item, auction->buyer );
        pay = ( int )auction->bet * 0.9;
        tax = ( int )auction->bet * 0.05;
        boost_economy( auction->seller->in_room->area, tax );
        auction->seller->gold += pay; /* give him the money, tax 10 %, lowered it to 5 %, im a cheap bastage -- Kal */
        ch_printf( auction->seller, "The auctioneer pays you %s zeni, charging an auction fee of ", num_punct( pay ) );
        ch_printf( auction->seller, "%s.\n\r", num_punct( tax ) );
        auction->item = NULL; /* reset item */
        if( IS_SET( sysdata.save_flags, SV_AUCTION ) )
        {
          save_char_obj( auction->buyer );
          save_char_obj( auction->seller );
        }
      }
      else  /* not sold */
      {
        sprintf( buf, "&cNo bids received for &C%s&c - removed from auction.", auction->item->short_descr );
        talk_auction( buf );
        act( AT_ACTION, "The auctioneer appears before you to return $p to you.",
             auction->seller, auction->item, NULL, TO_CHAR );
        act( AT_ACTION, "The auctioneer appears before $n to return $p to $m.",
             auction->seller, auction->item, NULL, TO_ROOM );
        if( ( auction->seller->carry_weight + get_obj_weight( auction->item ) ) > can_carry_w( auction->seller ) )
        {
          act( AT_PLAIN, "You drop $p as it is just too much to carry"
               " with everything else you're carrying.", auction->seller, auction->item, NULL, TO_CHAR );
          act( AT_PLAIN, "$n drops $p as it is too much extra weight"
               " for $m with everything else.", auction->seller, auction->item, NULL, TO_ROOM );
          obj_to_room( auction->item, auction->seller->in_room );
        }
        else
          obj_to_char( auction->item, auction->seller );
/*		tax = (int)auction->item->cost * 0.05; No.  Har har.  --Saiyr */
        tax = ( int )auction->starting * 0.05;
        boost_economy( auction->seller->in_room->area, tax );
        sprintf( buf, "The auctioneer charges you an auction fee of %s.\n\r", num_punct( tax ) );
        send_to_char( buf, auction->seller );
        if( ( auction->seller->gold - tax ) < 0 )
          auction->seller->gold = 0;
        else
          auction->seller->gold -= tax;
        if( IS_SET( sysdata.save_flags, SV_AUCTION ) )
          save_char_obj( auction->seller );
      } /* else */
      /*
       * auction que system -Goku 
       */
/*	    if (auction->freeQueSlot != -1)
	    {
	    	 make sure nothing has diapered, if something did then bump the auction que
	    	if(auction->queued_char[0] == NULL)
	    		while(auction->queued_char[0] == NULL)
	    		{
	    			bump_que();
	    			if (auction->freeQueSlot == -1)
	    			{
	    				auction->item = NULL;
	    				return;
	    			}
	    		}

			if(auction->queued_obj[0]->carried_by != auction->queued_char[0])
				while(auction->queued_obj[0]->carried_by != auction->queued_char[0])
	    		{
	    			bump_que();
	    			if (auction->freeQueSlot == -1)
	    			{
	    				auction->item = NULL;
	    				return;
	    			}
	    		}

			 everything's cool, start up the auction
			separate_obj(auction->queued_obj[0]);
			obj_from_char (auction->queued_obj[0]);
			if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
				save_char_obj(auction->queued_char[0]);
			auction->item = auction->queued_obj[0];
			auction->bet = 0;
			auction->buyer = auction->queued_char[0];
			auction->seller = auction->queued_char[0];
			auction->pulse = PULSE_AUCTION;
			auction->going = 0;
			auction->starting = auction->queued_starting_bid[0];
			sprintf (buf, "&cA new item is being auctioned: &C%s &cat &C%s zeni&c.", auction->item->short_descr, num_punct(auction->starting));
			talk_auction (buf);

			bump_que();
			return;
		}
*/
      auction->item = NULL; /* clear auction */
  } /* switch */

  sysdata.outBytesFlag = LOGBOUTNORM;
} /* func */

void subtract_times( struct timeval *etime, struct timeval *stime )
{
  etime->tv_sec -= stime->tv_sec;
  etime->tv_usec -= stime->tv_usec;
  while( etime->tv_usec < 0 )
  {
    etime->tv_usec += 1000000;
    etime->tv_sec--;
  }
  return;
}

/*
 * Function to update weather vectors according to climate
 * settings, random effects, and neighboring areas.
 * Last modified: July 18, 1997
 * - Fireblade
 */
void adjust_vectors( WEATHER_DATA * weather )
{
  NEIGHBOR_DATA *neigh;
  double dT, dP, dW;

  if( !weather )
  {
    bug( "adjust_vectors: NULL weather data.", 0 );
    return;
  }

  dT = 0;
  dP = 0;
  dW = 0;

  /*
   * Add in random effects 
   */
  dT += number_range( -rand_factor, rand_factor );
  dP += number_range( -rand_factor, rand_factor );
  dW += number_range( -rand_factor, rand_factor );

  /*
   * Add in climate effects
   */
  dT += climate_factor * ( ( ( weather->climate_temp - 2 ) * weath_unit ) - ( weather->temp ) ) / weath_unit;
  dP += climate_factor * ( ( ( weather->climate_precip - 2 ) * weath_unit ) - ( weather->precip ) ) / weath_unit;
  dW += climate_factor * ( ( ( weather->climate_wind - 2 ) * weath_unit ) - ( weather->wind ) ) / weath_unit;


  /*
   * Add in effects from neighboring areas 
   */
  for( neigh = weather->first_neighbor; neigh; neigh = neigh->next )
  {
    /*
     * see if we have the area cache'd already 
     */
    if( !neigh->address )
    {
      /*
       * try and find address for area 
       */
      neigh->address = get_area( neigh->name );

      /*
       * if couldn't find area ditch the neigh 
       */
      if( !neigh->address )
      {
        NEIGHBOR_DATA *temp;
        bug( "adjust_weather: " "invalid area name.", 0 );
        temp = neigh->prev;
        UNLINK( neigh, weather->first_neighbor, weather->last_neighbor, next, prev );
        STRFREE( neigh->name );
        DISPOSE( neigh );
        neigh = temp;
        continue;
      }
    }

    dT += ( neigh->address->weather->temp - weather->temp ) / neigh_factor;
    dP += ( neigh->address->weather->precip - weather->precip ) / neigh_factor;
    dW += ( neigh->address->weather->wind - weather->wind ) / neigh_factor;
  }

  /*
   * now apply the effects to the vectors 
   */
  weather->temp_vector += ( int )dT;
  weather->precip_vector += ( int )dP;
  weather->wind_vector += ( int )dW;

  /*
   * Make sure they are within the right range 
   */
  weather->temp_vector = URANGE( -max_vector, weather->temp_vector, max_vector );
  weather->precip_vector = URANGE( -max_vector, weather->precip_vector, max_vector );
  weather->wind_vector = URANGE( -max_vector, weather->wind_vector, max_vector );

  return;
}

/*
 * function updates weather for each area
 * Last Modified: July 31, 1997
 * Fireblade
 */
void weather_update(  )
{
  AREA_DATA *pArea;
  DESCRIPTOR_DATA *d;
  int limit;

  limit = 3 * weath_unit;

  for( pArea = first_area; pArea; pArea = ( pArea == last_area ) ? first_build : pArea->next )
  {
    /*
     * Apply vectors to fields 
     */
    pArea->weather->temp += pArea->weather->temp_vector;
    pArea->weather->precip += pArea->weather->precip_vector;
    pArea->weather->wind += pArea->weather->wind_vector;

    /*
     * Make sure they are within the proper range 
     */
    pArea->weather->temp = URANGE( -limit, pArea->weather->temp, limit );
    pArea->weather->precip = URANGE( -limit, pArea->weather->precip, limit );
    pArea->weather->wind = URANGE( -limit, pArea->weather->wind, limit );

    /*
     * get an appropriate echo for the area 
     */
    get_weather_echo( pArea->weather );
  }

  for( pArea = first_area; pArea; pArea = ( pArea == last_area ) ? first_build : pArea->next )
  {
    adjust_vectors( pArea->weather );
  }

  /*
   * display the echo strings to the appropriate players 
   */
  for( d = first_descriptor; d; d = d->next )
  {
    WEATHER_DATA *weath;

    if( d->connected == CON_PLAYING &&
        IS_OUTSIDE( d->character ) && !NO_WEATHER_SECT( d->character->in_room->sector_type ) && IS_AWAKE( d->character ) )
    {
      weath = d->character->in_room->area->weather;
      if( !weath->echo )
        continue;
      set_char_color( weath->echo_color, d->character );
      ch_printf( d->character, weath->echo );
    }
  }

  return;
}

/*
 * get weather echo messages according to area weather...
 * stores echo message in weath_data.... must be called before
 * the vectors are adjusted
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_weather_echo( WEATHER_DATA * weath )
{
  int n;
  int temp, precip, wind;
  int dT, dP, dW;
  int tindex, pindex, windex;

  /*
   * set echo to be nothing 
   */
  weath->echo = NULL;
  weath->echo_color = AT_GREY;

  /*
   * get the random number 
   */
  n = number_bits( 2 );

  /*
   * variables for convenience 
   */
  temp = weath->temp;
  precip = weath->precip;
  wind = weath->wind;

  dT = weath->temp_vector;
  dP = weath->precip_vector;
  dW = weath->wind_vector;

  tindex = ( temp + 3 * weath_unit - 1 ) / weath_unit;
  pindex = ( precip + 3 * weath_unit - 1 ) / weath_unit;
  windex = ( wind + 3 * weath_unit - 1 ) / weath_unit;

  /*
   * get the echo string... mainly based on precip 
   */
  switch ( pindex )
  {
    case 0:
      if( precip - dP > -2 * weath_unit )
      {
        char *echo_strings[4] = {
          "The clouds disappear.\n\r",
          "The clouds disappear.\n\r",
          "The sky begins to break through " "the clouds.\n\r",
          "The clouds are slowly " "evaporating.\n\r"
        };

        weath->echo = echo_strings[n];
        weath->echo_color = AT_WHITE;
      }
      break;

    case 1:
      if( precip - dP <= -2 * weath_unit )
      {
        char *echo_strings[4] = {
          "The sky is getting cloudy.\n\r",
          "The sky is getting cloudy.\n\r",
          "Light clouds cast a haze over " "the sky.\n\r",
          "Billows of clouds spread through " "the sky.\n\r"
        };
        weath->echo = echo_strings[n];
        weath->echo_color = AT_GREY;
      }
      break;

    case 2:
      if( precip - dP > 0 )
      {
        if( tindex > 1 )
        {
          char *echo_strings[4] = {
            "The rain stops.\n\r",
            "The rain stops.\n\r",
            "The rainstorm tapers " "off.\n\r",
            "The rain's intensity " "breaks.\n\r"
          };
          weath->echo = echo_strings[n];
          weath->echo_color = AT_CYAN;
        }
        else
        {
          char *echo_strings[4] = {
            "The snow stops.\n\r",
            "The snow stops.\n\r",
            "The snow showers taper " "off.\n\r",
            "The snow flakes disappear " "from the sky.\n\r"
          };
          weath->echo = echo_strings[n];
          weath->echo_color = AT_WHITE;
        }
      }
      break;

    case 3:
      if( precip - dP <= 0 )
      {
        if( tindex > 1 )
        {
          char *echo_strings[4] = {
            "It starts to rain.\n\r",
            "It starts to rain.\n\r",
            "A droplet of rain falls " "upon you.\n\r",
            "The rain begins to " "patter.\n\r"
          };
          weath->echo = echo_strings[n];
          weath->echo_color = AT_CYAN;
        }
        else
        {
          char *echo_strings[4] = {
            "It starts to snow.\n\r",
            "It starts to snow.\n\r",
            "Crystal flakes begin to " "fall from the " "sky.\n\r",
            "Snow flakes drift down " "from the clouds.\n\r"
          };
          weath->echo = echo_strings[n];
          weath->echo_color = AT_WHITE;
        }
      }
      else if( tindex < 2 && temp - dT > -weath_unit )
      {
        char *echo_strings[4] = {
          "The temperature drops and the rain " "becomes a light snow.\n\r",
          "The temperature drops and the rain " "becomes a light snow.\n\r",
          "Flurries form as the rain freezes.\n\r",
          "Large snow flakes begin to fall " "with the rain.\n\r"
        };
        weath->echo = echo_strings[n];
        weath->echo_color = AT_WHITE;
      }
      else if( tindex > 1 && temp - dT <= -weath_unit )
      {
        char *echo_strings[4] = {
          "The snow flurries are gradually " "replaced by pockets of rain.\n\r",
          "The snow flurries are gradually " "replaced by pockets of rain.\n\r",
          "The falling snow turns to a cold drizzle.\n\r",
          "The snow turns to rain as the air warms.\n\r"
        };
        weath->echo = echo_strings[n];
        weath->echo_color = AT_CYAN;
      }
      break;

    case 4:
      if( precip - dP > 2 * weath_unit )
      {
        if( tindex > 1 )
        {
          char *echo_strings[4] = {
            "The lightning has stopped.\n\r",
            "The lightning has stopped.\n\r",
            "The sky settles, and the " "thunder surrenders.\n\r",
            "The lightning bursts fade as " "the storm weakens.\n\r"
          };
          weath->echo = echo_strings[n];
          weath->echo_color = AT_GREY;
        }
      }
      else if( tindex < 2 && temp - dT > -weath_unit )
      {
        char *echo_strings[4] = {
          "The cold rain turns to snow.\n\r",
          "The cold rain turns to snow.\n\r",
          "Snow flakes begin to fall " "amidst the rain.\n\r",
          "The driving rain begins to freeze.\n\r"
        };
        weath->echo = echo_strings[n];
        weath->echo_color = AT_WHITE;
      }
      else if( tindex > 1 && temp - dT <= -weath_unit )
      {
        char *echo_strings[4] = {
          "The snow becomes a freezing rain.\n\r",
          "The snow becomes a freezing rain.\n\r",
          "A cold rain beats down on you " "as the snow begins to melt.\n\r",
          "The snow is slowly replaced by a heavy " "rain.\n\r"
        };
        weath->echo = echo_strings[n];
        weath->echo_color = AT_CYAN;
      }
      break;

    case 5:
      if( precip - dP <= 2 * weath_unit )
      {
        if( tindex > 1 )
        {
          char *echo_strings[4] = {
            "Lightning flashes in the " "sky.\n\r",
            "Lightning flashes in the " "sky.\n\r",
            "A flash of lightning splits " "the sky.\n\r",
            "The sky flashes, and the " "ground trembles with " "thunder.\n\r"
          };
          weath->echo = echo_strings[n];
          weath->echo_color = AT_YELLOW;
        }
      }
      else if( tindex > 1 && temp - dT <= -weath_unit )
      {
        char *echo_strings[4] = {
          "The sky rumbles with thunder as " "the snow changes to rain.\n\r",
          "The sky rumbles with thunder as " "the snow changes to rain.\n\r",
          "The falling turns to freezing rain " "amidst flashes of " "lightning.\n\r",
          "The falling snow begins to melt as " "thunder crashes overhead.\n\r"
        };
        weath->echo = echo_strings[n];
        weath->echo_color = AT_WHITE;
      }
      else if( tindex < 2 && temp - dT > -weath_unit )
      {
        char *echo_strings[4] = {
          "The lightning stops as the rainstorm " "becomes a blinding " "blizzard.\n\r",
          "The lightning stops as the rainstorm " "becomes a blinding " "blizzard.\n\r",
          "The thunder dies off as the " "pounding rain turns to " "heavy snow.\n\r",
          "The cold rain turns to snow and " "the lightning stops.\n\r"
        };
        weath->echo = echo_strings[n];
        weath->echo_color = AT_CYAN;
      }
      break;

    default:
      bug( "echo_weather: invalid precip index" );
      weath->precip = 0;
      break;
  }

  return;
}

/*
 * get echo messages according to time changes...
 * some echoes depend upon the weather so an echo must be
 * found for each area
 * Last Modified: August 10, 1997
 * Fireblade
 */
void get_time_echo( WEATHER_DATA * weath )
{
  int n;
  int pindex;

  n = number_bits( 2 );
  pindex = ( weath->precip + 3 * weath_unit - 1 ) / weath_unit;
  weath->echo = NULL;
  weath->echo_color = AT_GREY;

  switch ( time_info.hour )
  {
    case 5:
    {
      char *echo_strings[4] = {
        "The day has begun.\n\r",
        "The day has begun.\n\r",
        "The sky slowly begins to glow.\n\r",
        "The sun slowly embarks upon a new day.\n\r"
      };
      time_info.sunlight = SUN_RISE;
      weath->echo = echo_strings[n];
      weath->echo_color = AT_YELLOW;
      break;
    }
    case 6:
    {
      char *echo_strings[4] = {
        "The sun rises in the east.\n\r",
        "The sun rises in the east.\n\r",
        "The hazy sun rises over the horizon.\n\r",
        "Day breaks as the sun lifts into the sky.\n\r"
      };
      time_info.sunlight = SUN_LIGHT;
      weath->echo = echo_strings[n];
      weath->echo_color = AT_ORANGE;
      break;
    }
    case 12:
    {
      if( pindex > 0 )
      {
        weath->echo = "It's noon.\n\r";
      }
      else
      {
        char *echo_strings[2] = {
          "The intensity of the sun " "heralds the noon hour.\n\r",
          "The sun's bright rays beat down " "upon your shoulders.\n\r"
        };
        weath->echo = echo_strings[n % 2];
      }
      time_info.sunlight = SUN_LIGHT;
      weath->echo_color = AT_WHITE;
      break;
    }
    case 19:
    {
      char *echo_strings[4] = {
        "The sun slowly disappears in the west.\n\r",
        "The reddish sun sets past the horizon.\n\r",
        "The sky turns a reddish orange as the sun " "ends its journey.\n\r",
        "The sun's radiance dims as it sinks in the " "sky.\n\r"
      };
      time_info.sunlight = SUN_SET;
      weath->echo = echo_strings[n];
      weath->echo_color = AT_RED;
      break;
    }
    case 20:
    {
      if( pindex > 0 )
      {
        char *echo_strings[2] = {
          "The night begins.\n\r",
          "Twilight descends around you.\n\r"
        };
        weath->echo = echo_strings[n % 2];
      }
      else
      {
        char *echo_strings[2] = {
          "The moon's gentle glow diffuses " "through the night sky.\n\r",
          "The night sky gleams with " "glittering starlight.\n\r"
        };
        weath->echo = echo_strings[n % 2];
      }
      time_info.sunlight = SUN_DARK;
      weath->echo_color = AT_DBLUE;
      break;
    }
  }

  return;
}

/*
 * To change the phases of the moon
 */
char *get_moon_echo( WEATHER_DATA * weath )
{
  switch ( time_info.day )
  {
    case 6:
    {
      return ( "&WA quarter of the moon is visible.\n\r" );
    }
    case 13:
    {
      return ( "&WThe moon is completly dark.\n\r" );
    }
    case 14:
    {
      return ( "&RThe Makeo Star shines down with evil, blood red light.\n\r" );
    }
    case 21:
    {
      return ( "&WA quarter of the moon is visable.\n\r" );
    }
    case 29:
    {
      return ( "&YThe full moon shines brightly down on you.\n\r" );
    }
  }

  return ( "" );
}

/*
 * update the time
 */
void time_update(  )
{
  AREA_DATA *pArea;
  DESCRIPTOR_DATA *d;
  WEATHER_DATA *weath;
  struct tm *time;
  char *buf[MAX_STRING_LENGTH];

  *buf = NULL;
  time = localtime( &current_time );

  /*
   * Save MUD time once every hour 
   */
  save_timedata(  );

  switch ( ++time_info.hour )
  {
    case 5:
    case 6:
    case 12:
    case 19:
    case 20:
      for( pArea = first_area; pArea; pArea = ( pArea == last_area ) ? first_build : pArea->next )
      {
        get_time_echo( pArea->weather );
      }

      for( d = first_descriptor; d; d = d->next )
      {
        if( d->connected == CON_PLAYING && IS_OUTSIDE( d->character ) && IS_AWAKE( d->character ) )
        {
          weath = d->character->in_room->area->weather;
          if( !weath->echo )
            continue;
          set_char_color( weath->echo_color, d->character );
          ch_printf( d->character, weath->echo );
        }
      }
      break;
    case 24:
      time_info.hour = 0;
      time_info.day++;
      break;
  }

  if( time_info.hour == 20 && ( time_info.day == 6 || time_info.day == 13 || time_info.day == 21 || time_info.day == 29 ) )
  {
    for( pArea = first_area; pArea; pArea = ( pArea == last_area ) ? first_build : pArea->next )
    {
      *buf = get_moon_echo( pArea->weather );
    }

    for( d = first_descriptor; d; d = d->next )
    {
      if( d->connected == CON_PLAYING && IS_OUTSIDE( d->character ) && IS_AWAKE( d->character ) )
      {
        if( !buf )
          continue;
        pager_printf_color( d->character, *buf );
      }
    }
  }

  if( time_info.day >= 30 )
  {
    time_info.day = 0;
    time_info.month++;

  }

  if( time_info.month >= 17 )
  {
    time_info.month = 0;
    time_info.year++;
  }

  if( time_info.lastRealMonth != time->tm_mon )
  {
    ladderTableClear( time->tm_mon );

    time_info.lastRealMonth = time->tm_mon;
    save_timedata(  );
    save_hiscores(  );
    echo_to_all( AT_YELLOW, "The new ladder race has begun!", ECHOTAR_ALL );
  }
  return;
}
