/*****************************************/
/*  Reploid Transformations Version 1.0  */
/*             By Kalthizar              */
/*                                       */
/* [X] Active Form     [X] Power Form    */
/* [X] Proto Form      [X] Omega Form    */
/*****************************************/

#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );


#define AFF_OVERDRIVE


/*****************************************/
/*      Reploid Skills Version 2.0       */
/*             By Kalthizar              */
/*                                       */
/* [X] X Buster        [X] Z-Saber       */
/* [X] Leaf Shield     [X] Gaga Attack 1 */
/* [X] Giga Attack 2   [X] Twin Slashers */
/* [X] Frost Tower     [X] Ground Hunter */              
/* [ ] Soul Body       [X] Aiming Laser  */
/* [ ] Double Cyclone  [ ] Rising Fire   */
/* [ ] Lightning Web                     */
/*****************************************/

void do_xbuster(CHAR_DATA *ch, char *argument ) /* X Buster */
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_xbuster ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_xbuster]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_xbuster]->min_mana )
	{
	    send_to_char( "You don't have the Energy you fire your X Buster.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_xbuster]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_xbuster]->focus;

    WAIT_STATE( ch, skill_table[gsn_xbuster]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_xbuster ) )
    {
	dam = get_attmod(ch, victim) * number_range( 8, 10 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "You fire your X Buster at $N. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "$n fires at you with $s X Buster. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "$n fires at $N with $s X Buster. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_xbuster );
	learn_from_success( ch, gsn_xbuster );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "You missed $N with your X Buster.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "$n misses you with $s X Buster.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "$n missed $N with $s X Buster.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_xbuster );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_xbuster]->min_mana;
    return;
}

void do_zsaber(CHAR_DATA *ch, char *argument ) /* Z-Saber */
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_zsaber ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_zsaber]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_zsaber]->min_mana )
	{
	    send_to_char( "You don't have the energy to swing your Z Saber.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_zsaber]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_zsaber]->focus;

    WAIT_STATE( ch, skill_table[gsn_zsaber]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_zsaber ) )
    {
	dam = get_attmod(ch, victim) * number_range( 20, 25 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "Your left arm transforms into a shining &GGreen &Cplasma saber. ", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "You swing with all your might, cutting through $N. &W[$t]  ", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "$n's left arm transforms into a shining &GGreen &Cplasma saber. ", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "$n swings with all $s might, cutting through you. &W[$t]  ", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "$n's left arm transforms into a shining &GGreen &Cplasma saber. ", ch, num_punct(dam), victim, TO_NOTVICT );
	act( AT_CYAN, "$n swings with all $s might, cutting through $N. &W[$t]  ", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_zsaber );
	learn_from_success( ch, gsn_zsaber );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "You missed $N with your Z Saber.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "$n misses you with $s Z Saber.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "$n missed $N with $s Z Saber.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_zsaber );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_zsaber]->min_mana;
    return;
}

void do_lshield(CHAR_DATA *ch, char *argument ) /* Leaf Shield*/
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_lshield ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_lshield]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_lshield]->min_mana )
	{
	    send_to_char( "You don't have the energy to create a Leaf Shield.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_lshield]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_lshield]->focus;

    WAIT_STATE( ch, skill_table[gsn_lshield]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_lshield ) )
    {
	dam = get_attmod(ch, victim) * number_range( 15, 20 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "&GYou Summon the powers to Create Leaf Shield.", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "&GYou send it flying towards $N. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "&G$n creates a Leaf Shield and sends it flying at you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "&G$n creates a Leaf Shield and sends it flying towards $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_lshield );
	learn_from_success( ch, gsn_lshield );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "&GYou missed $N with your Leaf Shield.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "&G$n misses you with $s Leaf Shield.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "&G$n missed $N with $s Leaf Shield.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_lshield );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_lshield]->min_mana;
    return;
}

void do_giga1(CHAR_DATA *ch, char *argument ) /* Giga Attack 1*/
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_giga1 ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_giga1]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_giga1]->min_mana )
	{
	    send_to_char( "You don't have enough energy to do the Giga Attack.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_giga1]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_giga1]->focus;

    WAIT_STATE( ch, skill_table[gsn_giga1]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_giga1 ) )
    {
	dam = get_attmod(ch, victim) * number_range( 60, 65 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "&zYou look at $N with a blaze of Anger. In fit of hatred, you propel your body into the", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "&zair. Massive amounts of energy begin to jolt from your body, a meteor falls from the sky,", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "&zlanding just inches from $N's feet. As $N looks towards the sky, a whole barrage of the", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "&zred hot rocks are coming straight at $m. $N has no time to dodge and gets destroyed by them. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "&z$n looks at you with a blaze of Anger. In fit of hatred, $n propels $s body into the", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "&zair. Massive amounts of energy begin to jolt from $n's body, a meteor falls from the sky,", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "&zlanding just inches from your feet. As you look towards the sky, a whole barrage of the", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "&zred hot rocks are coming straight at you. You have no time to dodge and you are destroyed by them. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "&z$n looks at $N with a blaze of Anger. In fit of hatred, $n propels $s body into the", ch, num_punct(dam), victim, TO_NOTVICT );
	act( AT_CYAN, "&zair. Massive amounts of energy begin to jolt from $n's body, a meteor falls from the sky,", ch, num_punct(dam), victim, TO_NOTVICT );
	act( AT_CYAN, "&zlanding just inches from $N's feet. As $N looks towards the sky, a whole barrage of the", ch, num_punct(dam), victim, TO_NOTVICT );
	act( AT_CYAN, "&zred hot rocks are coming straight at $m. $N has no time to dodge and $N is destroyed by them. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_giga1 );
	learn_from_success( ch, gsn_giga1 );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "&GYou missed $N with your Giga Attack.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "&G$n misses you with $s Giga Attack.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "&G$n missed $N with $s Giga Attack.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_giga1 );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_giga1]->min_mana;
    return;
}

void do_giga2(CHAR_DATA *ch, char *argument ) /* Giga Attack 2 */
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_giga2 ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_giga2]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_giga2]->min_mana )
	{
	    send_to_char( "You don't have the Energy to use your Giga Attack.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_giga2]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_giga2]->focus;

    WAIT_STATE( ch, skill_table[gsn_giga2]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_giga2 ) )
    {
	dam = get_attmod(ch, victim) * number_range( 70, 80 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "As you launch yourself into the sky, wings begin to emerge from your body, propelling", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "you fowards. As this happens, a pale orangish aura begins emitting from your body, almost", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "as if you were charging for an energy blast. You slam through $N with a massive force. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "$n launches himself into the air, wings begin to emerge from $s body, propelling $m", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "forward. As this happens a pale orangish aura begins to envelope $s body, almost as", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "if $e were charging for an energy blast. $n slams through your body with a massive force. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "$n launches himself into the sky, wings emerge from $s body as $e propels $mself towards", ch, num_punct(dam), victim, TO_NOTVICT );
	act( AT_CYAN, "$N. A pale orangish aura evelopes $s body as $e flies through the air towards $N, $e hits with", ch, num_punct(dam), victim, TO_NOTVICT );
 	act( AT_CYAN, "with such a massive force, you feel as if $e hit you as well. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
       dam = ki_absorb( victim, ch, dam, gsn_giga2 );
	learn_from_success( ch, gsn_giga2 );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "You missed $N with your Giga Attack.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "$n misses you with $s Giga Attack.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "$n missed $N with $s Giga Attack.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_giga2 );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_giga2]->min_mana;
    return;
}

void do_tslasher(CHAR_DATA *ch, char *argument ) /* Twin Slasher X4 */
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_tslasher ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_tslasher]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_tslasher]->min_mana )
	{
	    send_to_char( "You don't have the Energy you fire your Twin Slasher.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_tslasher]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_tslasher]->focus;

    WAIT_STATE( ch, skill_table[gsn_tslasher]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_tslasher ) )
    {
	dam = get_attmod(ch, victim) * number_range( 30, 35 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "Adapting the powers of Slash Beast, you fire your Twin Slashers at $N. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "$n fires at you with $s Twin Slasher. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "$n fires at $N with $s Twin Slasher. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_tslasher );
	learn_from_success( ch, gsn_tslasher );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "You missed $N with your Twin Slasher.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "$n misses you with $s Twin Slasher.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "$n missed $N with $s Twin Slasher.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_tslasher );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_tslasher]->min_mana;
    return;
}

void do_ftower(CHAR_DATA *ch, char *argument ) /* Frost Tower X4 */
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_ftower ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_ftower]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_ftower]->min_mana )
	{
	    send_to_char( "You don't have the Energy you fire your Frost Tower.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_ftower]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_ftower]->focus;

    WAIT_STATE( ch, skill_table[gsn_ftower]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_ftower ) )

    {
	dam = get_attmod(ch, victim) * number_range( 25, 35 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "Adapting the powers of Frost Walrus, you surround your body in Ice, inflicting damage to $N as $E grabs you. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "$n's body turns to ice as you grab $m. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "$n's body turns to ice as $N grabs $m. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        xSET_BIT( (ch)->affected_by, AFF_ICESHIELD ); 
        dam = ki_absorb( victim, ch, dam, gsn_ftower );
	learn_from_success( ch, gsn_ftower );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "You missed $N with your Frost Tower.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "$n misses you with $s Frost Tower.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "$n missed $N with $s Frost Tower.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_ftower );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_ftower]->min_mana;
    return;
}

void do_ghunter(CHAR_DATA *ch, char *argument ) /* Ground Hunter X4 */
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_ghunter ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_ghunter]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_ghunter]->min_mana )
	{
	    send_to_char( "You don't have the Energy you fire your Ground Hunter.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_ghunter]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_ghunter]->focus;

    WAIT_STATE( ch, skill_table[gsn_ghunter]->beats );

    //sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_ghunter ) )

    {
	dam = get_attmod(ch, victim) * number_range( 35, 40 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_CYAN, "Adapting the powers of Jet Sting Ray, you fire a miniture sting ray at $N, it explodes on impact. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_CYAN, "$n fires a miniture sting ray at you, it hits. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_CYAN, "$n fires a miniture sting ray at $N, it hits. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_ghunter );
	learn_from_success( ch, gsn_ghunter );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_CYAN, "&GYou missed $N with your Ground Hunter.", ch, NULL, victim, TO_CHAR );
	act( AT_CYAN, "&G$n misses you with $s Ground Hunter.", ch, NULL, victim, TO_VICT );
	act( AT_CYAN, "&G$n missed $N with $s Ground Hunter.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_ghunter );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_ghunter]->min_mana;
    return;
}

void do_aiminglaser( CHAR_DATA * ch, char *argument ) /* Aiming Laser */
{
  CHAR_DATA *victim;
  int dam = 0;
  int count = 0;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_aiminglaser ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_aiminglaser]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  count = atoi( argument );

  if( count < 1 || count > 20 )
  {
    ch_printf( ch, "The batch of lasers must be between 1 and 20.\n\r" );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_aiminglaser]->min_mana * count )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_aiminglaser]->focus * count )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_aiminglaser]->focus * count;

  WAIT_STATE( ch, skill_table[gsn_aiminglaser]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_aiminglaser ) )
  {
    int shots = count;
    int dam2 = 0;

    while( shots > 0 )
    {
      switch ( number_range( 1, 3 ) )
      {
        default:
        case 1:
        case 2:
          dam += number_range( 1, 2 );
          break;
        case 3:
          dam += number_range( 1, 3 );
          break;
      }
      shots--;
    }
    dam2 = number_range( 4, 8 );
    //dam2 = number_range( 8, 20 );
    dam = ( get_attmod( ch, victim ) * dam * dam2 );

    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    strcpy( buf, num_punct( count ) );
    strcpy( buf2, num_punct( dam ) );

    act2( AT_RED,
          "You cross your arms infront of yourself and reroute an incredible amount of energy from around your body; your aura flaring up brightly as you do. Your arm suddenly fills up with this energy as it transforms into a strange laser cannon. You fire $t energy beams towards $N.",
          ch, buf, victim, TO_CHAR, NULL );
    act2( AT_RED,
          "You hold your arm steady with your free hand, keeping a lock on $N. All $t lasers home in on $N and pierce $M, exploding violently. &W[$T]",
          ch, buf, victim, TO_CHAR, buf2 );
    act2( AT_RED,
          "$n crosses $s arms infront of $mself and reroutes an incredible amount of energy from around $s body; $s aura flaring up brightly as $e does. $n's arm suddenly fills up with this energy as it transforms into a strange laser cannon. $n fires $t energe beams towards you.",
          ch, buf, victim, TO_VICT, NULL );
    act2( AT_RED,
          "$n holds $s arm steady with $s free hand, keeping a lock on you. All $t lasers home in on you and pierce your body, exploding violently. &W[$T]",
          ch, buf, victim, TO_VICT, buf2 );
    act2( AT_RED,
          "$n crosses $s arms infront of $mself and reroutes an incredible amount of energy from around $s body; $s aura flaring up brightly as $e does. $n's arm suddenly fills up with this energy as it transforms into a strange laser cannon. $n fires $t energe beams towards $N.",
          ch, buf, victim, TO_NOTVICT, NULL );
    act2( AT_RED,
          "$n holds $s arm steady with $s free hand, keeping a lock on $N. All $t lasers home in on $N and pierce $M, exploding violently. &W[$T]",
          ch, buf, victim, TO_NOTVICT, buf2 );

    dam = ki_absorb( victim, ch, dam, gsn_aiminglaser );
    learn_from_success( ch, gsn_aiminglaser );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_RED, "You missed $N with your aiming laser.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s aiming laser.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with $s aiming laser.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_aiminglaser );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_aiminglaser]->min_mana * count;
  return;
}

void do_pabsorb( CHAR_DATA * ch, char *argument )
{
  float left = 0;
  float right = 0;

  if( IS_NPC( ch ) )
    return;

  left = ( float )ch->mana / ch->max_mana;
  right = ( float )ch->pcdata->learned[gsn_pabsorb] / 100;

  if( ch->fighting )
  {
    send_to_char( "&wYou can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( left >= right )
  {
    send_to_char( "&zYou are already at peace with everything.\n\r", ch );
    return;
  }

  switch ( ch->substate )
  {
    default:
      if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
      {
        send_to_char( "&wYou can't concentrate enough for that.\n\r", ch );
        return;
      }
      if( ch->mount )
      {
        send_to_char( "&wYou can't do that while mounted.\n\r", ch );
        return;
      }
      add_timer( ch, TIMER_DO_FUN, number_range( 1, 3 ), do_pabsorb, 1 );
      send_to_char( "&zYou begin to absorb photons...\n\r", ch );
      act( AT_PLAIN, "&z$n begins to absorb photons...", ch, NULL, NULL, TO_ROOM );
      return;

    case 1:
      if( ch->fighting )
      {
        send_to_char( "&zYou stop absorbing photons...\n\r", ch );
        act( AT_PLAIN, "&z$n stops absorbing photons...", ch, NULL, NULL, TO_ROOM );
        return;
      }

      if( can_use_skill( ch, number_percent(  ), gsn_pabsorb ) )
      {
        send_to_char( "&zYou absorb photons into your body, replenishing lost energy.\n\r", ch );
        learn_from_success( ch, gsn_meditate );
        ch->mana += ( float )right / 25 * ch->max_mana;
      }
      else
      {
        send_to_char( "&zYou fail to absorb enough photons.\n\r", ch );
        learn_from_failure( ch, gsn_meditate );
      }

      if( left >= right )
      {
        send_to_char( "&zYou have no need for further replenishing energy.\n\r", ch );
        return;
      }

      add_timer( ch, TIMER_DO_FUN, number_range( 1, 3 ), do_pabsorb, 2 );
      return;
    case 2:
      if( ch->fighting )
      {
        send_to_char( "&zYou stop absorbing photons...\n\r", ch );
        act( AT_PLAIN, "&z$n stops absorbing photons...", ch, NULL, NULL, TO_ROOM );
        return;
      }

      if( can_use_skill( ch, number_percent(  ), gsn_pabsorb ) )
      {
        send_to_char( "&zYou absorb photons into your body, replenishing lost energy.\n\r", ch );
        learn_from_success( ch, gsn_meditate );
        ch->mana += ( float )right / 25 * ch->max_mana;
      }
      else
      {
        send_to_char( "&zYou fail to absorb enough photons.\n\r", ch );
        learn_from_failure( ch, gsn_pabsorb );
      }

      if( left >= right )
      {
        send_to_char( "&zYou have no need for further replenishing energy.\n\r", ch );
        return;
      }

      add_timer( ch, TIMER_DO_FUN, number_range( 1, 3 ), do_pabsorb, 1 );
      return;
    case SUB_TIMER_DO_ABORT:
      ch->substate = SUB_NONE;
      send_to_char( "&zYou stop absorbing photons...\n\r", ch );
      act( AT_PLAIN, "&z$n stops absorbing photons...", ch, NULL, NULL, TO_ROOM );
      return;
  }
  return;
}


/* Overdrive Mode: unified transformation for Active/Power/Proto/Omega */

void do_overdrive_mode(CHAR_DATA *ch, char *argument)
{
    int arg = atoi(argument);
    /* Check if any individual form flag is set */
    
    bool inForm = (
        xIS_SET(ch->affected_by, AFF_ACTIVE_FORM) ||
        xIS_SET(ch->affected_by, AFF_POWER_FORM)  ||
        xIS_SET(ch->affected_by, AFF_PROTO_FORM)  ||
        xIS_SET(ch->affected_by, AFF_OMEGA_FORM)
    );
        /* —— old‑school toggle‑off if already powered up —— */
        if ( inForm )
        {
            send_to_char(
                "You rapidly power down from Overdrive, rerouting all systems to standby.\n\r",
                ch );
            xREMOVE_BIT( ch->affected_by, AFF_ACTIVE_FORM );
            xREMOVE_BIT( ch->affected_by, AFF_POWER_FORM  );
            xREMOVE_BIT( ch->affected_by, AFF_PROTO_FORM  );
            xREMOVE_BIT( ch->affected_by, AFF_OMEGA_FORM  );
            ch->in_room->light--;
            transStatRemove( ch );
            ch->pl = ch->exp;
            return;
        }
    double pl_mult;
    int tier;
    bool success;
    int use_gsn;

    /* NPC master split check */
    if (IS_NPC(ch) && is_split(ch)) {
        if (!ch->master
            || !can_use_skill(ch->master, number_percent(), gsn_active_form))
            return;
    }

    /* Block if any chip installed */
    if (wearing_chip(ch) || wearing_sentient_chip(ch)) {
        ch_printf(ch, "Your auxiliary systems must be offline before Overdrive engages.\n\r");
        return;
    }

    /* Toggle off with arg 0 */
    if (arg == 0) {
        if (inForm) {
            send_to_char(
                "You reroute your energy cores back to standby, phasing out any active Overdrive Form.\n\r",
                ch);
            /* Clear all form bits */
            xREMOVE_BIT(ch->affected_by, AFF_ACTIVE_FORM);
            xREMOVE_BIT(ch->affected_by, AFF_POWER_FORM);
            xREMOVE_BIT(ch->affected_by, AFF_PROTO_FORM);
            xREMOVE_BIT(ch->affected_by, AFF_OMEGA_FORM);
            ch->in_room->light--;
            transStatRemove(ch);
            ch->pl = ch->exp;
            if (!IS_NPC(ch)) {
                ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
                ch->pcdata->eyes      = ch->pcdata->orignaleyes;
            }
        } else {
            ch_printf(ch, "Syntax: overdrive mode [12-40]\n\r");
        }
        return;
    }

    /* Enforce full range: 12-40 */
    if (arg < 12) {
        ch_printf(ch, "Syntax: overdrive mode [12-40]\n\r");
        return;
    }
    if (arg > 40) {
        send_to_char("&RWarning:&z Your frame cannot handle that level of Overdrive power.\n\r", ch);
        return;
    }
    pl_mult = (double)arg;

    /* Select appropriate skill slot */
    if (pl_mult <= 18)
        use_gsn = gsn_active_form;
    else if (pl_mult <= 28)
        use_gsn = gsn_power_form;
    else
        use_gsn = gsn_omega_form;

    /* Mana requirement check */
    if (ch->mana < skill_table[use_gsn]->min_mana * pl_mult/4) {
        send_to_char("&RWarning:&z Insufficient energy reserves for Overdrive Mode.\n\r", ch);
        return;
    }

    /* Cooldown */
    WAIT_STATE(ch, skill_table[use_gsn]->beats);
    success = can_use_skill(ch, number_percent(), use_gsn);

    if (success && !inForm) {
        /* Determine appearance tier across 7 brackets */
        tier = (int)((pl_mult - 12) / 4);
        if (tier > 7) tier = 7;

        /* Tier-specific description */
        switch (tier) {
            case 0:
                act(AT_CYAN,
                    "Your plating glows faint azure as Overdrive circuits engage.",
                    ch, NULL, NULL, TO_CHAR);
                break;
            case 1:
                act(AT_CYAN,
                    "Cyan veins pulse brighter, servos hum with precision.",
                    ch, NULL, NULL, TO_CHAR);
                break;
            case 2:
                act(AT_GREEN,
                    "Emerald conduits flare, surging strength through your frame.",
                    ch, NULL, NULL, TO_CHAR);
                break;
            case 3:
                act(AT_GREEN,
                    "Lime arcs envelop joints, agility heightened significantly.",
                    ch, NULL, NULL, TO_CHAR);
                break;
            case 4:
                act(AT_YELLOW,
                    "Golden plating locks, balancing power and speed flawlessly.",
                    ch, NULL, NULL, TO_CHAR);
                break;
            case 5:
                act(AT_YELLOW,
                    "Amber pulses race through you, performance beyond specs.",
                    ch, NULL, NULL, TO_CHAR);
                break;
            case 6:
                act(AT_RED,
                    "Crimson energy roils in core, systems at maximum capacity.",
                    ch, NULL, NULL, TO_CHAR);
                break;
            case 7:
                act(AT_WHITE,
                    "Blinding white aura envelops you as Overdrive peaks.",
                    ch, NULL, NULL, TO_CHAR);
                break;
        }

        /* Activate Overdrive by setting only the requested form bit */
        if (use_gsn == gsn_active_form)
            xSET_BIT(ch->affected_by, AFF_ACTIVE_FORM);
        else if (use_gsn == gsn_power_form)
            xSET_BIT(ch->affected_by, AFF_POWER_FORM);
        else
            xSET_BIT(ch->affected_by, AFF_OMEGA_FORM);

        /* Clear others */
        if (use_gsn != gsn_active_form)
            xREMOVE_BIT(ch->affected_by, AFF_ACTIVE_FORM);
        if (use_gsn != gsn_power_form)
            xREMOVE_BIT(ch->affected_by, AFF_POWER_FORM);
        if (use_gsn != gsn_proto_form)
            xREMOVE_BIT(ch->affected_by, AFF_PROTO_FORM);
        if (use_gsn != gsn_omega_form)
            xREMOVE_BIT(ch->affected_by, AFF_OMEGA_FORM);

        /* Light up the room */
        ch->in_room->light++;
        /* Set power level */
        ch->pl = ch->exp * pl_mult;
        /* Apply stats */
        transStatApply(ch,
                       (int)(-0.2 * pl_mult),
                       (int)( 0.5 * pl_mult),
                       (int)( 0.4 * pl_mult),
                       (int)( 0.3 * pl_mult));

        learn_from_success(ch, use_gsn);
    } else if (!success && !inForm) {
        act(AT_RED,
            "Your systems sputter—Overdrive Mode fails to initialize.",
            ch, NULL, NULL, TO_CHAR);
        learn_from_failure(ch, use_gsn);
    }

    /* Energy cost */
    ch->mana -= skill_table[use_gsn]->min_mana * pl_mult/4;
}
