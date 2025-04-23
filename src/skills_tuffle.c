#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"
void transStatApply(CHAR_DATA *ch, int str, int spd, int con, int intt);
void do_supersymbiote(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;



    if (xIS_SET(ch->affected_by, AFF_SUPERSYMBIOTE)) {
        act(AT_DGREY, "You feel the writhing energy retreat as your parasitic form subsides.", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "$n's corrupted form flickers before reverting to its base state.", ch, NULL, NULL, TO_ROOM);
        xREMOVE_BIT(ch->affected_by, AFF_SUPERSYMBIOTE);
        ch->pl = ch->exp;
        transStatRemove(ch);
        return;
    }

    if (xIS_SET(ch->affected_by, AFF_KAIOKEN)) {
        send_to_char("You can't control your parasitic energy while using Kaioken.\n\r", ch);
        return;
    }

    if (ch->mana < skill_table[gsn_supersymbiote]->min_mana) {
        send_to_char("You don't have enough energy.\n\r", ch);
        return;
    }

    WAIT_STATE(ch, skill_table[gsn_supersymbiote]->beats);

    if (can_use_skill(ch, number_percent(), gsn_supersymbiote)) {
        act(AT_DGREY, "A shudder tears through your body as parasitic tendrils erupt beneath your skin.", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "Your eyes flicker gold as corrupted power surges through your form.", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "You roar as your body is enveloped in a writhing storm of shadow-infused light!", ch, NULL, NULL, TO_CHAR);

        act(AT_DGREY, "$n convulses violently, shadow tendrils piercing through $s skin as $e transforms.", ch, NULL, NULL, TO_ROOM);
        act(AT_DGREY, "$n's eyes ignite with golden corruption as a parasitic aura floods the room.", ch, NULL, NULL, TO_ROOM);
        act(AT_DGREY, "$n lets out a distorted, echoing roar as $s form becomes an abomination of stolen might.", ch, NULL, NULL, TO_ROOM);

        xSET_BIT(ch->affected_by, AFF_SUPERSYMBIOTE);
        ch->pl = ch->exp * 13; // Matched with SSJ1
        transStatApply(ch, 14, 14, 12, 12);
        learn_from_success(ch, gsn_supersymbiote);
    }
    else {
        act(AT_DGREY, "You strain to awaken your parasitic strength, but it recoils violently within.", ch, NULL, NULL, TO_CHAR);
        act(AT_DGREY, "$n snarls in frustration as their body flickers with unstable black light.", ch, NULL, NULL, TO_ROOM);
        learn_from_failure(ch, gsn_supersymbiote);
    }

    ch->mana -= skill_table[gsn_supersymbiote]->min_mana;
}
