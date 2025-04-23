#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );

void do_innerwill( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(), gsn_innerwill ) )
      return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this.\n\r" );
    return;
  }

  if( xIS_SET( ch->affected_by, AFF_INNERWILL ) )
  {
    send_to_char( "&zYou let out a final snarl as your energy dies down, your form returning to normal.\n\r", ch );
    xREMOVE_BIT( ch->affected_by, AFF_INNERWILL );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }

  if( xIS_SET( ch->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't use the Inner Will technique while using kaioken.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_innerwill]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_innerwill]->beats );

  if( can_use_skill( ch, number_percent(), gsn_innerwill ) )
  {
    act( AT_DGREY, "Dark energy crackles around your body, and you begin to transform...", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "You roar in fury as jet black scales momentarily flicker across your skin.", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "Your eyes ignite with hatred, blazing crimson as your power surges upward!", ch, NULL, NULL, TO_CHAR );

    act( AT_DGREY, "Dark energy crackles around $n's body as a shadowy aura rises around them.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "$n lets out a feral roar, black scales rippling briefly across their form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "$n's eyes flare red as a malevolent surge of power floods the area.", ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ch->affected_by, AFF_INNERWILL );
    ch->pl = ch->exp * 13;  // Same as SSJ1 multiplier
    transStatApply( ch, 14, 14, 12, 12 );  // Comparable to SSJ1 boost
    learn_from_success( ch, gsn_innerwill );
  }
  else
  {
    act( AT_DGREY, "You try to summon your inner will, but the rage slips away too soon.", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "$n grits their teeth and shakes with effort, but nothing happens.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_innerwill );
  }

  ch->mana -= skill_table[gsn_innerwill]->min_mana;
}

void do_mechaevolution(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;


    if (ch->exp < 10000000 || ch->pl < ch->exp) {
        send_to_char("You lack the power and energy to evolve into this form.\n\r", ch);
        return;
    }

    if (xIS_SET(ch->affected_by, AFF_MECHAEVO)) {
        act(AT_DGREY, "&zWith a sharp hiss of decompressing pressure, your mechanical form collapses into dust and shadow.", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "&z$n's mechanical form collapses inward with a hiss, fading back into a twisted organic state.", ch, NULL, NULL, TO_NOTVICT);
        xREMOVE_BIT(ch->affected_by, AFF_MECHAEVO);
        ch->pl = ch->exp;
        transStatRemove(ch);
        return;
    }

    if (!xIS_SET(ch->affected_by, AFF_INNERWILL)) {
        send_to_char("You must first awaken your Inner Will before evolving.\n\r", ch);
        return;
    }

    WAIT_STATE(ch, skill_table[gsn_mechaevolution]->beats);

    if (can_use_skill(ch, number_percent(), gsn_mechaevolution)) {
      act(AT_RED, "&RYour muscles seize and twist as blackened steel rips through your flesh, reforging your body into a weapon of pure devastation!", ch, NULL, NULL, TO_CHAR);
      act(AT_RED, "&RJagged plates of obsidian metal burst from beneath your skin, encasing you in a living fortress of wrath and power!", ch, NULL, NULL, TO_CHAR);
      act(AT_RED, "&ROne final surge of corrupted ki erupts from your core as your transformation completes—&YMecha Evolution has begun!&D", ch, NULL, NULL, TO_CHAR);
  
      act(AT_RED, "&R$n howls with mechanical fury as thick metal tendrils lash from their back, piercing the air with a thunderous shriek!", ch, NULL, NULL, TO_NOTVICT);
      act(AT_RED, "&RWith a violent shockwave, $n's body is remade—blackened steel plates snap into place as their aura ignites in blazing crimson arcs!", ch, NULL, NULL, TO_NOTVICT);
      act(AT_RED, "&RThe ground trembles beneath $n as the &YMecha Evolution&D surges into existence!", ch, NULL, NULL, TO_NOTVICT);

        xSET_BIT(ch->affected_by, AFF_MECHAEVO);
        ch->pl = ch->exp * 15; // Between SSJ2 and SSJ3
        transStatApply(ch, 20, 12, 12, 20); // STR, INT, SPD, CON
        heart_calc(ch, "");
        learn_from_success(ch, gsn_mechaevolution);
    } else {
      act(AT_RED, "&rYou growl as internal systems surge with corrupted energy, but the evolution aborts mid-sequence!", ch, NULL, NULL, TO_CHAR);
      act(AT_RED, "&rA sharp shriek of metal echoes as your synthetic core flickers and dies, unable to complete the transformation.", ch, NULL, NULL, TO_CHAR);

      act(AT_RED, "&r$n convulses violently, surrounded by a storm of sparks and distortion—but the evolution fails to ignite.", ch, NULL, NULL, TO_NOTVICT);
      act(AT_RED, "&rA wave of failed energy lashes outward from $n, as their form remains unchanged.", ch, NULL, NULL, TO_NOTVICT);
        learn_from_failure(ch, gsn_mechaevolution);
    }
}

void do_ancientevolution(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;



    if (xIS_SET(ch->affected_by, AFF_ANCIENTEVO)) {
        send_to_char("You have already attained your ultimate evolution.\n\r", ch);
        return;
    }

    if (!xIS_SET(ch->affected_by, AFF_MECHAEVO)) {
        send_to_char("You must first undergo your Mecha Evolution before reaching this form.\n\r", ch);
        return;
    }

    if (xIS_SET(ch->affected_by, AFF_KAIOKEN)) {
        send_to_char("You cannot channel the Ancient Evolution while using Kaioken.\n\r", ch);
        return;
    }

    if (ch->mana < skill_table[gsn_ancientevolution]->min_mana) {
        send_to_char("You lack the shadow energy to sustain such a monstrous form.\n\r", ch);
        return;
    }

    WAIT_STATE(ch, skill_table[gsn_ancientevolution]->beats);

    if (can_use_skill(ch, number_percent(), gsn_ancientevolution)) {
        act(AT_DGREY, "&zA vortex of shadow energy engulfs you as your form begins to contort and swell.&D", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "&zJagged, obsidian scales burst from your skin as your body expands with brutal energy.&D", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "&zYour howl echoes like thunder through the void as the ground quakes beneath you.&D", ch, NULL, NULL, TO_CHAR);

        act(AT_DGREY, "&z$n is swallowed by a maelstrom of black energy that warps the very air around them.&D", ch, NULL, NULL, TO_NOTVICT);
        act(AT_DGREY, "&zCracks of violet lightning arc outward as $n's body grows, armored in nightmarish scales.&D", ch, NULL, NULL, TO_NOTVICT);
        act(AT_DGREY, "&zA deafening roar splits the heavens as $n completes their Ancient Evolution!&D", ch, NULL, NULL, TO_NOTVICT);

        xSET_BIT(ch->affected_by, AFF_ANCIENTEVO);
        ch->pl = ch->exp * 30; // Matching SSJGod multiplier
        transStatApply(ch, 25, 20, 20, 25); // STR, INT, SPD, CON boost
        learn_from_success(ch, gsn_ancientevolution);
    } else {
        act(AT_DGREY, "&zYou summon the storm within, but the dark current twists violently out of control.&D", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "&zYour body convulses, shadow energy searing your veins before vanishing in a burst of heat.&D", ch, NULL, NULL, TO_CHAR);

        act(AT_DGREY, "&z$n trembles violently, black lightning crackling around them before abruptly vanishing.&D", ch, NULL, NULL, TO_NOTVICT);
        act(AT_DGREY, "&z$n stumbles back as their failed transformation dissipates into smoke.&D", ch, NULL, NULL, TO_NOTVICT);

        learn_from_failure(ch, gsn_ancientevolution);
    }

    ch->mana -= skill_table[gsn_ancientevolution]->min_mana;
}

void do_flare(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_flare))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_flare]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_flare]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_flare]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_flare]->focus;

    WAIT_STATE( ch, skill_table[gsn_flare]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_flare ) )
    {
	switch (number_range(1,100))
	{
    case 100:
    case 99:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR)) ))
 	{

		/* Redone so it can't be used to kill players
		 * above 5x their pl. Was going to do 2x like
		 * DDD, but if I did that, it'd remove the challenge
		 * from Coolers. -- Islvin
		 */
		if (victim->pl / ch->pl >= 2)
	  {
		act( AT_RED, "RYou clench your fists and fire begins rushing through your"
      	       	   "veins as you rush at $n. You grab them tightly as a sphere of fire" 
	             	   "forms around the both of you, letting them go as they get trapped" 
	                 "inside with you. Closing your eyes, you concentrate hard and increase"
	                 "the temperature of the sphere tenfold, opening your eyes again to watch" 
	                 "$T writh in searing agony. Suddenly you scream 'Now you DIE!'," 
	                 "leaving them unaffected!!!", ch, NULL, victim, TO_CHAR );
		act( AT_RED, "$N clenches his fists and fire begins rushing through $s veins as $e rushes"
	                 "at you. $e grabs you tightly as a sphere of fire forms around the both of you"
	                 "breaking free of $s grip as you get trapped inside. Closing $s eyes, $e" 
	                 "concentrates hard and increases the temperature of the sphere tenfold, opening" 
	                 "$s eyes again to watch you writh in searing agony. Suddenly $e screams 'Now you DIE!'"
	                 "the sphere explodes, leaving you unaffected", ch, NULL, victim, TO_VICT );
		act( AT_RED, "$N clenches $s fists and fire begins rushing through $s veins as $e rushes at $T. $e" 
	                 "grabs them tightly as a sphere of fire forms around the both of them. You see $N close" 
	                 "$s eyes and you feel the temperature radiating from the sphere increase tenfold as $n" 
	                 "writhes in pain. Suddenly $N screams 'Now you DIE!', and the sphere explodes, leaving" 
	                 "them unaffected.", ch, NULL, victim, TO_NOTVICT );
		dam = 0;
		break;
	  }

/* Redo damage so it _is_ instant death
 * Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
		dam = 999999999;
	act( AT_RED, "RYou clench your fists and fire begins rushing through your"
                 "veins as you rush at $n. You grab them tightly as a sphere of fire" 
                 "forms around the both of you, letting them go as they get trapped" 
                 "inside with you. Closing your eyes, you concentrate hard and increase"
                 "the temperature of the sphere tenfold, opening your eyes again to watch" 
                 "$T writh in searing agony. Suddenly you scream 'Now you DIE!'," 
                 "INCINERATING THEM INSTANTLY!!!", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$N clenches his fists and fire begins rushing through $s veins as $e rushes"
                 "at you. $e grabs you tightly as a sphere of fire forms around the both of you"
                 "breaking free of $s grip as you get trapped inside. Closing $s eyes, $e" 
                 "concentrates hard and increases the temperature of the sphere tenfold, opening" 
                 "$s eyes again to watch you writh in searing agony. Suddenly $e screams 'Now you DIE!'"
                 "the sphere explodes, INCINERATING YOU INSTANTLY", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$N clenches $s fists and fire begins rushing through $s veins as $e rushes at $T. $e" 
                 "grabs them tightly as a sphere of fire forms around the both of them. You see $N close" 
                 "$s eyes and you feel the temperature radiating from the sphere increase tenfold as $n" 
                 "writhes in pain. Suddenly $N screams 'Now you DIE!', and the sphere explodes, INCINERATING" 
                 "THEM INSTANTLY.", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_flare );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
	}
	default:
	dam = get_attmod(ch, victim) * number_range( 55, 65 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_RED, "You clench your fists and fire begins rushing through your veins as you rush at $N. You grab"
                 "them tightly as a sphere of fire forms around the both of you, letting them go as they get" 
                 "trapped inside with you. Closing your eyes, you concentrate hard and increase the temperature" 
                 "of the sphere tenfold, opening your eyes again to watch $N writh in searing agony. Suddenly "
                 "you scream 'Now you DIE!', the sphere explodes, sending $N sprawling to the floor. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "$N clenches his fists and fire begins rushing through $s veins as $e rushes at you. $e grabs you"
                 "tightly as a sphere of fire forms around the both of you, breaking free of $s grip as you get trapped"
                 "inside. Closing $s eyes, $e concentrates hard and increases the temperature of the sphere tenfold"
                 "opening $s eyes again to watch you writh in searing agony. Suddenly $e screams 'Now you DIE!', the" 
                 "sphere explodes, sending you sprawling to the floor.&W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "$N clenches $s fists and fire begins rushing through $s veins as $e rushes at $n. $e grabs them"
                 "tightly as a sphere of fire forms around the both of them. You see $N close $s eyes and you feel the"
                 "temperature radiating from the sphere increase tenfold as $n writhes in pain. Suddenly $N screams 'Now"
                 "you DIE!', and the sphere explodes, sending $n sprawling to the floor. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_flare );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
   	}
    }
    else
    {
	act( AT_RED, "You missed $N with your flare attack.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s flare attack.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with a flare attack.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_flare );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_flare]->min_mana;
    return;
}

void do_atb(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_atb ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_atb]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_atb]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_atb]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_atb]->focus;

    WAIT_STATE( ch, skill_table[gsn_atb]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_atb ) )
    {
	dam = get_attmod(ch, victim) * number_range( 120, 140 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_DGREY, "You raise both your palms high into the air, facing upwards,", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "as electricty surges up your arms. A black ball of energy", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "it. The ball grows to a massive size and density so extreme", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "atomized space remains. This ball contains all the negative", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "you yell at $N, flinging the Antimatter Bomb at $M. $E", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "tries to block the massive ball, but the resulting explosion", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "can be seen from miles away and leaves the area utterly vaporized. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "atomized space remains. This ball contains all the negative", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$e yells at you, flinging the Antimatter Bomb at you. You", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "try to block the massive ball, but the resulting explosion", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "can be seen from miles away and leaves the area utterly vaporized. &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "atomized space remains. This ball contains all the negative", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "$n yells at $N, flinging the Antimatter Bomb at them. $N", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "tries to block the massive ball, but the resulting explosion", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "can be seen from miles away and leaves the area utterly vaporized. &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_atb );
	learn_from_success( ch, gsn_atb );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_DGREY, "You raise both your palms high into the air, facing upwards,", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "as electricty surges up your arms. A black ball of energy", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "it. The ball grows to a massive size and density so extreme", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "atomized space remains. The ball contains all the negative", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "you yell at $N, flinging the Antimatter Bomb at $M. $E", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "tries to block the massive ball, but before $E can, the", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "antimatter bomb fizzles out. &W[&REPIC FAIL&W]", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "atomized space remains. The ball contains all the negative", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$e yells at you, flinging the Antimatter Bomb at you. You", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "tries to block the massive ball, but before you can, the", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "antimatter bomb fizzles out. &W[&REPIC FAIL&W]", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "atomized space remains. The ball contains all the negative", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "$n yells at $N, flinging the Antimatter Bomb at them. $N", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "tries to block the massive ball, but before $E can, the", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "antimatter bomb fizzles out. &W[&REPIC FAIL&W]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_atb );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_atb]->min_mana;
    return;
}
void do_gust(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_gust ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_gust]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	  if( !xIS_SET( ( ch )->affected_by, AFF_ANCIENTEVO ) )
	  {
	    send_to_char( "You can't use a wing-based attack without wings. Doh!\n\r", ch );
	    return;
	  }

	if ( ch->mana < skill_table[gsn_gust]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_gust]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
/*	if ( ch->affected_by =! ancientevolution )
	{
		send_to_char( "You need to be in your Ancient form!\n\r", ch );
		return;
	}
*/
    else
    	ch->focus -= skill_table[gsn_gust]->focus;

    WAIT_STATE( ch, skill_table[gsn_gust]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_gust ) )
    {
	dam = get_attmod(ch, victim) * number_range( 50, 59 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_WHITE, "You flex your wings and launch several feet into the air. "
			"You let out a horrible shriek as you beat your wings, sending "
			"torrents of air and debris flying at $N. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at you! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at $N! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
			learn_from_success( ch, gsn_gust );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_WHITE, "You flex your wings and launch several feet into the air. "
			"You let out a horrible shriek as you beat your wings, sending "
			"torrents of air and debris flying at $N.. but wait, where'd they "
			"go? &W[&RUSUX&W]", ch, NULL, victim, TO_CHAR );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at you, but you quickly get "
			"out of the path of the attack. &W[&RUSUX&W]", ch, NULL, victim, TO_VICT );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at $N, but $N quickly gets "
			"out of the path of the attack. &W[&RUSUX&W]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_gust );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_gust]->min_mana;
    return;
}
void do_buffet(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_buffet ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_buffet]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_buffet]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_buffet]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
/*	if ( ch->affected_by =! ancientevolution )
	{
		send_to_char( "You need to be in your Ancient form!\n\r", ch );
		return;
	}
*/
    else
    	ch->focus -= skill_table[gsn_buffet]->focus;

    WAIT_STATE( ch, skill_table[gsn_buffet]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_buffet ) )
    {
	dam = get_attmod(ch, victim) * number_range( 60, 69 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_WHITE, "You beat your wings and lift off into the air. Accelerating "
			"to your maximum speed, you perform a wide arc in the air above "
			"$N then turn sharply, headed straight towards them. When you are "
			"just about upon them, you straigten out your wings and brake hard, "
			"sending an extremely powerful buffet of wind and debris crashing "
			"into them! &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_WHITE, "$n beats their wings and lifts off into the air. Accelerating "
			"to $s maximum speed, $e performs a wide arc in the air above you, "
			"then turns sharply and heads straight for you! When $e is almost "
			"upon you, $e straightens out $s wings and brakes hard, sending an "
			"extremely powerful buffet of wind and debris crashing into you! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_WHITE, "$n beats their wings and lifts off into the air. Accelerating "
			"to $s maximum speed, $e performs a wide arc in the air above $N, "
			"then turns sharply and heads straight for them! When $e is almost "
			"upon $N, $e straightens out $s wings and brakes hard, sending an "
			"extremely powerful buffet of wind and debris crashing into them! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
			learn_from_success( ch, gsn_buffet );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_WHITE, "You prepare to lift off, but suddenly your wings malfunction. &G[&rCRAMP!!&G]", ch, NULL, victim, TO_CHAR );
	act( AT_WHITE, "$n prepares to lift off, but suddenly $s wings malfunction. &G[&rCRAMP!!&G]", ch, NULL, victim, TO_VICT );
	act( AT_WHITE, "$n prepares to lift off, but suddenly $s wings malfunction. &G[&rCRAMP!!&G]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_buffet );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_buffet]->min_mana;
    return;
}
void do_dragonfire(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_dragonfire ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_dragonfire]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_dragonfire]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_dragonfire]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
     else
    	ch->focus -= skill_table[gsn_dragonfire]->focus;

    WAIT_STATE( ch, skill_table[gsn_dragonfire]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_dragonfire ) )
    {
	switch (number_range(1,100))
	{
    case 100:
    case 99:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR)) ))
	{
	if (victim->pl / ch->pl >= 2)
	{
	act( AT_RED, "You draw your head back to gather some Dragon Fire, but "
			"$N presses the attack and you are forced to abandon the attempt.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "You close too quickly and $n abandons their attempt at a "
			"Dragon Fire, adopting a defensive stance.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$N closes too quickly and $n abandons their attempt at a "
			"Dragon Fire, adopting a defensive stance.", ch, NULL, victim, TO_NOTVICT );
	dam = 0;
	break;
	}
       dam= 999;
	act( AT_RED, "You lift off into the air, hovering several feet above $N. You glare "
			"at them as you gather Dragon Fire in your throat. Feeling the napalm-like "
			"fire about to overflow, you spew a gigantic stream at them. When the smoke "
			"clears, a large crater of molten lava is all that remains of the area.\n\r"
			"\n\r&W[&rINS&RT&WA&RK&rILL!&W]&D", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n lifts off into the air, hovering several feet above you. You catch "
			"their glare just before they spew a gigantic stream of Dragon Fire at "
			"you. You have no time to react, and you feel intense heat just before "
			"everything goes white.\n\r"
			"\n\r&W[&rINS&RT&WA&RK&rILL!&W]&D", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n lifts off into the air, hovering several feet above $N. $N catches "
			"$n's glare just before $e spews a gigantic stream of Dragon Fire at "
			"$N. $N has no time to react, and they feel intense heat just before "
			"everything goes white. When the smoke clears, a large crater of molten "
			"lava is all that remains of the area.\n\r"
			"\n\r&W[&rINS&RT&WA&RK&rILL!&W]&D", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_dragonfire );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	break;
	}
	default:
	dam = get_attmod(ch, victim) * number_range( 70, 80 );

	act( AT_RED, "You lift off into the air, hovering several feet above $N. You glare "
			"at them as you gather Dragon Fire in your throat. Feeling the napalm-like "
			"fire about to overflow, you spew a gigantic stream at them. When the smoke "
			"clears, $N is horribly burned. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "$n lifts off into the air, hovering several feet above you. You catch "
			"their glare just before they spew a gigantic stream of Dragon Fire at "
			"you. You cannot avoid it but you at least manage not to get hit directly. "
			"Nevertheless, you feel your skin bubbling as you are horribly burned! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "$n lifts off into the air, hovering several feet above $N. $N catches "
			"$n's glare just before $e spews a gigantic stream of Dragon Fire at "
			"$N. $N cannot avoid it but they at least manage not to get hit directly."
			"Nevertheless, $N feels their skin bubbling as they are horribly burned! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_dragonfire );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
   break;
     }
    }
    else
    {
	act( AT_RED, "You miss $N with your Dragon Fire! &w[&GBURN!!&w]", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s Dragon Fire! &w[&GBURN!!&w]", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n misses $N with Dragon Fire! &w[&GBURN!!&w]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_dragonfire );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_dragonfire]->min_mana;

    return;
}

void do_dragon_thunder(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_dragon_thunder ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_dragon_thunder]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_dragon_thunder]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_dragon_thunder]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }

    else
    	ch->focus -= skill_table[gsn_dragon_thunder]->focus;

    WAIT_STATE( ch, skill_table[gsn_dragon_thunder]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_dragon_thunder ) )
    {
	dam = get_attmod(ch, victim) * number_range( 60, 70 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_ORANGE, "You fold your arms across your chest, jumping back a few feet "
		"as $N goes off balance. You swing your arms forward, attacking in  "
            "$N's moment of weakness, you shout, '&RDragon Thunder!&O' A giant, bright "
            "&Ww&wh&Wi&wt&We&O bolt of lightning rockets down from the sky and strikes $N in a "
		"breathtaking fury! &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_ORANGE, "$n folds $s arms across $s chest, jumping back a few feet "
		"as you go off balance. $n flings $s arms forward, attacking in "
            "your moment of weakness, shouting, '&RDragon Thunder!&O' A giant, bright "
            "&Ww&wh&Wi&wt&We&O bolt of lightning rockets down from the sky and strikes you in "
		"a breathtaking fury! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_ORANGE, "$n folds $s arms across $s chest, jumping back a few feet as "
		"$N goes off balance. $n flings $s arms forward, attacking in $N's "
            "moment of weakness, shouting, '&RDragon Thunder!&O' A giant, bright &Ww&wh&Wi&wt&We&O "
		"bolt of lightning rockets down from the sky and strikes $N! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
	learn_from_success( ch, gsn_dragon_thunder );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_ORANGE, "You missed $N with your dragon thunder. &z[&YSHOCKING!&z]", ch, NULL, victim, TO_CHAR );
	act( AT_ORANGE, "$n missed you with $s dragon thunder. &z[&YSHOCKING!&z]", ch, NULL, victim, TO_VICT );
	act( AT_ORANGE, "$n misses $N with a dragon thunder. &z[&YSHOCKING!&z]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_dragon_thunder );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_dragon_thunder]->min_mana;
    return;
}

void do_whirlwind_spin(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_whirlwind_spin ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_whirlwind_spin]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_whirlwind_spin]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_whirlwind_spin]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }

    else
    	ch->focus -= skill_table[gsn_whirlwind_spin]->focus;

    WAIT_STATE( ch, skill_table[gsn_whirlwind_spin]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_whirlwind_spin ) )
    {
	dam = get_attmod(ch, victim) * number_range( 110, 115 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_DGREY, "You cross your arms at the wrist, making a strange 'V' with  "
		"your hands. A mass of crackling, white air forms in the small space  "
            "between your hands. You jump away from $N's attack, crossing your hands "
            "in an 'X' to unsleash the still building attack. A jagged sphere of air  "
            "splashes against $N, causing their body and clothes to be ripped to shreads "
	     " as the power washes over $N. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_DGREY, "$n crosses their arms at the wrist, making a strange 'V' with their "
		"hands. A mass of crackling, white air forms in the small space between their "
              "hands. They jump away from your attack, crossing their hands in an 'X' to unleash "
              "the building attack. A jagged sphere of air splashes against you, causing your "
		"body and clothes to be ripped to shreads as the power washes over you. &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_DGREY, "$n crosses their arms at the wrist, making a strange 'V' with their hands. "
                      "A mass of crackling, white air forms in the small space between $s hands "
                      "as $n jumps away from $N's attack, crossing their hands in an 'X' to unleash "
                      "the still building attack. A jagged sphere of air splashes against $N, "
		"causing their body and clothes to be ripped to shreds as the power washes over them. &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
	learn_from_success( ch, gsn_whirlwind_spin );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_DGREY, "You missed $N with your whirldwind spin. &G[&zSit & Spin&G]", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "$n missed you with $s whirldwind spin. &G[&zSit & Spin&G]", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$n misses $N with a whirldwind spin. &G[&zSit & Spin&G]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_whirlwind_spin );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_whirlwind_spin]->min_mana;
    return;
}

void do_gasattack(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_gasattack))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_gasattack]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
 	if (IS_NPC(victim) || (!IS_NPC(victim) && xIS_SET(ch->act, PLR_SPAR)) || xIS_SET(ch->act, PLR_SPAR) )
	{
	    send_to_char( "&RPKILL ONLY N00B - Kal.\n\r", ch );
	    return;
	}
	if ( ch->mana < skill_table[gsn_gasattack]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_gasattack]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_gasattack]->focus;

    WAIT_STATE( ch, skill_table[gsn_gasattack]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_gasattack ) )
    {
	dam = get_attmod(ch, victim) * number_range( 10, 15 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_GREEN, "You close your eyes and begin to focus your energy as you breath in real deep and release"
                 "it slowly and methodically. What once was air that entered your lungs is now exhaled as" 
                 "a large cloud of neurotoxic gases. It slowly grows large enough, envolping $N and yourself." 
                 "You slowly open your eyes and watch as $N begins to buckle under the toxins power. As"
                 "time withers away, so does $S transformation, until $E is completely powerd down. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_GREEN, "$n closes $s eyes and begins to focus $s energy as $e breathes in real deep and releases"
                 "it slowly and methodically. What once was air that entered $s lungs is now exhaled as" 
                 "a large cloud of neurotoxic gases. It slowly grows large enough, envolping you and $n." 
                 "$n slowly opens $s eyes and watches as you begin to buckle under the toxins power. As"
                 "time withers away, so does your transformation, until you is completely powerd down. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_GREEN, "$n closes $s eyes and begins to focus $s energy as $e breathes in real deep and releases"
                 "it slowly and methodically. What once was air that entered $s lungs is now exhaled as" 
                 "a large cloud of neurotoxic gases. It slowly grows large enough, envolping $N and $n." 
                 "$n slowly opens $s eyes and watches as $N begins to buckle under the toxins power. As"
                 "time withers away, so does $N's transformation, until $E is completely powerd down. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_gasattack );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ ) )
      {
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ );
        if( !IS_NPC( victim ) )
        {
          ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
          ch->pcdata->eyes = ch->pcdata->orignaleyes;
        }
      }
      if( xIS_SET( ( victim )->affected_by, AFF_USSJ ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_USSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_USSJ2 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_USSJ2 );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ2 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ2 );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ3 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ3 );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ4 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ4 );
      if( xIS_SET( ( victim )->affected_by, AFF_KAIOKEN ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_KAIOKEN );
      if( xIS_SET( ( victim )->affected_by, AFF_SNAMEK ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SNAMEK );
      if( xIS_SET( ( victim )->affected_by, AFF_HYPER ) )
        xREMOVE_BIT( victim->affected_by, AFF_HYPER );
      if( xIS_SET( ( victim )->affected_by, AFF_EXTREME ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_EXTREME );
      if( xIS_SET( ( victim )->affected_by, AFF_GROWTH ) )
        xREMOVE_BIT( victim->affected_by, AFF_GROWTH );
      if( xIS_SET( ( victim )->affected_by, AFF_GIANT ) )
        xREMOVE_BIT( victim->affected_by, AFF_GIANT );
      if( xIS_SET( ( victim )->affected_by, AFF_EVIL_TRANS ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVIL_TRANS );
      if( xIS_SET( ( victim )->affected_by, AFF_SUPER_TRANS ) )
        xREMOVE_BIT( victim->affected_by, AFF_SUPER_TRANS );
      if( xIS_SET( ( victim )->affected_by, AFF_KID_TRANS ) )
        xREMOVE_BIT( victim->affected_by, AFF_KID_TRANS );
      if( xIS_SET( ( victim )->affected_by, AFF_MYSTIC ) )
        xREMOVE_BIT( victim->affected_by, AFF_MYSTIC );
      if( xIS_SET( ( victim )->affected_by, AFF_LSSJ ) )
        xREMOVE_BIT( victim->affected_by, AFF_LSSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_MAJIN ) )
        xREMOVE_BIT( victim->affected_by, AFF_MAJIN );
      if( xIS_SET( ( victim )->affected_by, AFF_SIGMA_VIRUS ) )
        xREMOVE_BIT( victim->affected_by, AFF_SIGMA_VIRUS );
      if( xIS_SET( ( victim )->affected_by, AFF_SENT_CHIP ) )
        xREMOVE_BIT( victim->affected_by, AFF_SENT_CHIP );
      if( xIS_SET( ( victim )->affected_by, AFF_EVILBOOST ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVILBOOST );
      if( xIS_SET( ( victim )->affected_by, AFF_EVILSURGE ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVILSURGE );
      if( xIS_SET( ( victim )->affected_by, AFF_EVILOVERLOAD ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVILOVERLOAD );
      if( xIS_SET( ( victim )->affected_by, AFF_UNAMEK ) )
        xREMOVE_BIT( victim->affected_by, AFF_UNAMEK );
      if( xIS_SET( ( victim )->affected_by, AFF_DEMBRACE ) )
        xREMOVE_BIT( victim->affected_by, AFF_DEMBRACE );
      if( xIS_SET( ( victim )->affected_by, AFF_ARAPTURE ) )
        xREMOVE_BIT( victim->affected_by, AFF_ARAPTURE );
      if( xIS_SET( ( victim )->affected_by, AFF_SERALIGHT ) )
        xREMOVE_BIT( victim->affected_by, AFF_SERALIGHT );
      if( xIS_SET( ( victim )->affected_by, AFF_SUPER_OOZARU ) )
        xREMOVE_BIT( victim->affected_by, AFF_SUPER_OOZARU );
      if( xIS_SET( ( victim )->affected_by, AFF_HSSJ ) )
        xREMOVE_BIT( victim->affected_by, AFF_HSSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_ESSJ ) )
        xREMOVE_BIT( victim->affected_by, AFF_ESSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_SAIBAMAN1 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SAIBAMAN1 );
      if( xIS_SET( ( victim )->affected_by, AFF_SAIBAMAN2 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SAIBAMAN2 );
      if( xIS_SET( ( victim )->affected_by, AFF_SAIBAMAN3 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SAIBAMAN3 );
      if( xIS_SET( ( victim )->affected_by, AFF_PUPPET ) )
        xREMOVE_BIT( victim->affected_by, AFF_PUPPET );
      if( xIS_SET( ( victim )->affected_by, AFF_SFLAME1 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SFLAME1 );
      if( xIS_SET( ( victim )->affected_by, AFF_SFLAME2 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SFLAME2 );	
      if( xIS_SET( ( victim )->affected_by, AFF_SFLAME3 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SFLAME3 );
      if( xIS_SET( ( victim )->affected_by, AFF_UFLAME ) )
        xREMOVE_BIT( victim->affected_by, AFF_UFLAME );
      if( xIS_SET( ( victim )->affected_by, AFF_ADULT ) )
        xREMOVE_BIT( victim->affected_by, AFF_ADULT );
      if( xIS_SET( ( victim )->affected_by, AFF_DEKU ) )
        xREMOVE_BIT( victim->affected_by, AFF_DEKU );
      if( xIS_SET( ( victim )->affected_by, AFF_GORON ) )
        xREMOVE_BIT( victim->affected_by, AFF_GORON );
      if( xIS_SET( ( victim )->affected_by, AFF_ZORA ) )
        xREMOVE_BIT( victim->affected_by, AFF_ZORA );	
      if( xIS_SET( ( victim )->affected_by, AFF_FDEITY ) )
        xREMOVE_BIT( victim->affected_by, AFF_FDEITY );
      if( xIS_SET( ( victim )->affected_by, AFF_INNERWILL ) )
        xREMOVE_BIT( victim->affected_by, AFF_INNERWILL );
      if( xIS_SET( ( victim )->affected_by, AFF_MECHAEVO ) )
        xREMOVE_BIT( victim->affected_by, AFF_MECHAEVO );	
      if( xIS_SET( ( victim )->affected_by, AFF_ANCIENTEVO ) )
        xREMOVE_BIT( victim->affected_by, AFF_ANCIENTEVO );
      if( !is_android( victim ) && !is_superandroid( victim ) && !IS_BIOANDROID( victim ) && !is_reploid( victim ) && !IS_ICER( victim ) )
      {
        ch->powerup = 0;
        if( xIS_SET( ( victim )->affected_by, AFF_HEART ) )
          xREMOVE_BIT( victim->affected_by, AFF_HEART );
        transStatRemove( victim );
        victim->pl = victim->exp;
      }
    }
    else
    {
	act( AT_GREEN, "Your neurotoxic gas left $N unaffected.", ch, NULL, victim, TO_CHAR );
	act( AT_GREEN, "$n's neurotoxic gas leaves you unaffected.", ch, NULL, victim, TO_VICT );
	act( AT_GREEN, "$n's neurotoxic gas left $N unaffected.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_gasattack );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_gasattack]->min_mana;
    return;
}
