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
*			   Player communication module                     *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef REGEX
#include <regex.h>
#endif

#ifdef FREEBSD
#include <unistd.h>
#include <regex.h>
#endif
#include "mud.h"


#ifdef REGEX
extern int re_exec _RE_ARGS( ( const char * ) );
#endif

/*
#ifndef WIN32
  #include <regex.h>
#endif
*/

/*
 *  Externals
 */
void send_obj_page_to_char( CHAR_DATA * ch, OBJ_INDEX_DATA * idx, char page );
void send_room_page_to_char( CHAR_DATA * ch, ROOM_INDEX_DATA * idx, char page );
void send_page_to_char( CHAR_DATA * ch, MOB_INDEX_DATA * idx, char page );
void send_control_page_to_char( CHAR_DATA * ch, char page );
void mpchkbot( CHAR_DATA * victim, char *argument );
int unread_notes( CHAR_DATA * ch, GLOBAL_BOARD_DATA * board );
void rank_update args( ( void ) );

/*
 * Local functions.
 */
void talk_channel args( ( CHAR_DATA * ch, char *argument, int channel, const char *verb ) );

char *scramble args( ( const char *argument, int modifier ) );
char *drunk_speech args( ( const char *argument, CHAR_DATA * ch ) );
void chatperform args( ( CHAR_DATA * ch, CHAR_DATA * victim, char *msg, int trigfrom ) );
void chatperformtoroom args( ( char *txt, CHAR_DATA * ch ) );

/*
 *  Profanity handler stuff  (forgive me for what i must do)
 */
void add_profane_word( char *word );
int is_profane( char *what );
char *bigregex = NULL;
char *preg;
bool can_admin args( ( CHAR_DATA * ch ) );
void undeaf_admin args( (  ) );

bool long_str_prefix( char *astr, char *bstr )
{
  int length = 0;
  int x = 0;

  if( !astr )
  {
    bug( "long_str_prefix: null astr." );
    return TRUE;
  }

  if( !bstr )
  {
    bug( "long_str_prefix: null bstr." );
    return TRUE;
  }

  length = strlen( bstr );

  for( x = 0; x < length; x++ )
  {
    if( LOWER( astr[x] ) != LOWER( bstr[x] ) )
      return TRUE;
  }

  return FALSE;
}


char *strip_char_prefix( char *aString, int count )
{
  static char bString[MAX_STRING_LENGTH];
  int length = 0;
  int x = 0;
  int i = 0;

  if( !aString || count == 0 )
    return aString;

  bString[0] = '\0';

  length = strlen( aString );

  if( length > MAX_STRING_LENGTH )
    length = MAX_STRING_LENGTH;
  if( count > MAX_STRING_LENGTH )
    count = MAX_STRING_LENGTH;

  for( x = count; x <= length; x++ )
  {
    bString[i] = aString[x];
    i++;
  }

  length = strlen( bString );
  if( bString[length] != '\0' )
    bString[length + 1] = '\0';

  return ( bString );
}

bool can_admin( CHAR_DATA * ch )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *vch;

  for( d = first_descriptor; d; d = d->next )
  {
    vch = d->character;

    if( !vch )
    {
      continue;
    }

    if( IS_IMMORTAL( vch ) && !xIS_SET( vch->deaf, CHANNEL_ADMIN ) && !xIS_SET( vch->act, PLR_AFK ) )
    {
      return TRUE;
    }
  }

  return FALSE;
}

void undeaf_admin(  )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *vch;

  for( d = first_descriptor; d; d = d->next )
  {
    vch = d->character;

    if( !vch )
    {
      continue;
    }

    xREMOVE_BIT( vch->deaf, CHANNEL_ADMIN );

  }
  return;
}

/* Text scrambler -- Altrag */
char *scramble( const char *argument, int modifier )
{
  static char arg[MAX_INPUT_LENGTH];
  sh_int position;
  sh_int conversion = 0;

  modifier %= number_range( 80, 300 );  /* Bitvectors get way too large #s */
  for( position = 0; position < MAX_INPUT_LENGTH; position++ )
  {
    if( argument[position] == '\0' )
    {
      arg[position] = '\0';
      return arg;
    }
    else if( argument[position] >= 'A' && argument[position] <= 'Z' )
    {
      conversion = -conversion + position - modifier + argument[position] - 'A';
      conversion = number_range( conversion - 5, conversion + 5 );
      while( conversion > 25 )
        conversion -= 26;
      while( conversion < 0 )
        conversion += 26;
      arg[position] = conversion + 'A';
    }
    else if( argument[position] >= 'a' && argument[position] <= 'z' )
    {
      conversion = -conversion + position - modifier + argument[position] - 'a';
      conversion = number_range( conversion - 5, conversion + 5 );
      while( conversion > 25 )
        conversion -= 26;
      while( conversion < 0 )
        conversion += 26;
      arg[position] = conversion + 'a';
    }
    else if( argument[position] >= '0' && argument[position] <= '9' )
    {
      conversion = -conversion + position - modifier + argument[position] - '0';
      conversion = number_range( conversion - 2, conversion + 2 );
      while( conversion > 9 )
        conversion -= 10;
      while( conversion < 0 )
        conversion += 10;
      arg[position] = conversion + '0';
    }
    else
      arg[position] = argument[position];
  }
  arg[position] = '\0';
  return arg;
}


LANG_DATA *get_lang( const char *name )
{
  LANG_DATA *lng;

  for( lng = first_lang; lng; lng = lng->next )
    if( !str_cmp( lng->name, name ) )
      return lng;
  return NULL;
}

/* percent = percent knowing the language. */
char *translate( int percent, const char *in, const char *name )
{
  LCNV_DATA *cnv;
  static char buf[256];
  char buf2[256];
  const char *pbuf;
  char *pbuf2 = buf2;
  LANG_DATA *lng;

  if( percent > 99 || !str_cmp( name, "common" ) )
    return ( char * )in;

  /*
   * If we don't know this language... use "default" 
   */
  if( !( lng = get_lang( name ) ) )
    if( !( lng = get_lang( "default" ) ) )
      return ( char * )in;

  for( pbuf = in; *pbuf; )
  {
    for( cnv = lng->first_precnv; cnv; cnv = cnv->next )
    {
      if( !str_prefix( cnv->old, pbuf ) )
      {
        if( percent && ( rand(  ) % 100 ) < percent )
        {
          strncpy( pbuf2, pbuf, cnv->olen );
          pbuf2[cnv->olen] = '\0';
          pbuf2 += cnv->olen;
        }
        else
        {
          strcpy( pbuf2, cnv->new );
          pbuf2 += cnv->nlen;
        }
        pbuf += cnv->olen;
        break;
      }
    }
    if( !cnv )
    {
      if( isalpha( *pbuf ) && ( !percent || ( rand(  ) % 100 ) > percent ) )
      {
        *pbuf2 = lng->alphabet[LOWER( *pbuf ) - 'a'];
        if( isupper( *pbuf ) )
          *pbuf2 = UPPER( *pbuf2 );
      }
      else
        *pbuf2 = *pbuf;
      pbuf++;
      pbuf2++;
    }
  }
  *pbuf2 = '\0';
  for( pbuf = buf2, pbuf2 = buf; *pbuf; )
  {
    for( cnv = lng->first_cnv; cnv; cnv = cnv->next )
      if( !str_prefix( cnv->old, pbuf ) )
      {
        strcpy( pbuf2, cnv->new );
        pbuf += cnv->olen;
        pbuf2 += cnv->nlen;
        break;
      }
    if( !cnv )
      *( pbuf2++ ) = *( pbuf++ );
  }
  *pbuf2 = '\0';
#if 0

  for( pbuf = in, pbuf2 = buf; *pbuf && *pbuf2; pbuf++, pbuf2++ )
    if( isupper( *pbuf ) )
      *pbuf2 = UPPER( *pbuf2 );
  /*
   * Attempt to align spacing.. 
   */
    else if( isspace( *pbuf ) )
      while( *pbuf2 && !isspace( *pbuf2 ) )
        pbuf2++;
#endif

  return buf;
}


char *drunk_speech( const char *argument, CHAR_DATA * ch )
{
  const char *arg = argument;
  static char buf[MAX_INPUT_LENGTH * 2];
  char buf1[MAX_INPUT_LENGTH * 2];
  sh_int drunk;
  char *txt;
  char *txt1;

  if( IS_NPC( ch ) || !ch->pcdata )
    return ( char * )argument;

  drunk = ch->pcdata->condition[COND_DRUNK];

  if( drunk <= 0 )
    return ( char * )argument;

  buf[0] = '\0';
  buf1[0] = '\0';

  if( !argument )
  {
    bug( "Drunk_speech: NULL argument", 0 );
    return "";
  }

  /*
   * if ( *arg == '\0' )
   * return (char *) argument;
   */

  txt = buf;
  txt1 = buf1;

  while( *arg != '\0' )
  {
    if( toupper( *arg ) == 'T' )
    {
      if( number_percent(  ) < ( drunk * 2 ) )  /* add 'h' after an 'T' */
      {
        *txt++ = *arg;
        *txt++ = 'h';
      }
      else
        *txt++ = *arg;
    }
    else if( toupper( *arg ) == 'X' )
    {
      if( number_percent(  ) < ( drunk * 2 / 2 ) )
      {
        *txt++ = 'c', *txt++ = 's', *txt++ = 'h';
      }
      else
        *txt++ = *arg;
    }
    else if( number_percent(  ) < ( drunk * 2 / 5 ) ) /* slurred letters */
    {
      sh_int slurn = number_range( 1, 2 );
      sh_int currslur = 0;

      while( currslur < slurn )
        *txt++ = *arg, currslur++;
    }
    else
      *txt++ = *arg;

    arg++;
  };

  *txt = '\0';

  txt = buf;

  while( *txt != '\0' ) /* Let's mess with the string's caps */
  {
    if( number_percent(  ) < ( 2 * drunk / 2.5 ) )
    {
      if( isupper( *txt ) )
        *txt1 = tolower( *txt );
      else if( islower( *txt ) )
        *txt1 = toupper( *txt );
      else
        *txt1 = *txt;
    }
    else
      *txt1 = *txt;

    txt1++, txt++;
  };

  *txt1 = '\0';
  txt1 = buf1;
  txt = buf;

  while( *txt1 != '\0' )  /* Let's make them stutter */
  {
    if( *txt1 == ' ' )  /* If there's a space, then there's gotta be a */
    { /* along there somewhere soon */

      while( *txt1 == ' ' ) /* Don't stutter on spaces */
        *txt++ = *txt1++;

      if( ( number_percent(  ) < ( 2 * drunk / 4 ) ) && *txt1 != '\0' )
      {
        sh_int offset = number_range( 0, 2 );
        sh_int pos = 0;

        while( *txt1 != '\0' && pos < offset )
          *txt++ = *txt1++, pos++;

        if( *txt1 == ' ' )  /* Make sure not to stutter a space after */
        { /* the initial offset into the word */
          *txt++ = *txt1++;
          continue;
        }

        pos = 0;
        offset = number_range( 2, 4 );
        while( *txt1 != '\0' && pos < offset )
        {
          *txt++ = *txt1;
          pos++;
          if( *txt1 == ' ' || pos == offset ) /* Make sure we don't stick */
          { /* A hyphen right before a space */
            txt1--;
            break;
          }
          *txt++ = '-';
        }
        if( *txt1 != '\0' )
          txt1++;
      }
    }
    else
      *txt++ = *txt1++;
  }

  *txt = '\0';

  return buf;
}

/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA * ch, char *argument, int channel, const char *verb )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf_invis[MAX_STRING_LENGTH];
  char buf_notag[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int dinum = 0;
  int dival = 0;
  int position;
  CHAR_DATA *admVict = NULL;
  char admName[MAX_STRING_LENGTH];

#ifndef SCRAMBLE

  int speaking = -1, lang;

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif

  if( IS_NPC( ch ) && is_split( ch ) )
    return;

  if( IS_NPC( ch ) && channel == CHANNEL_CLAN )
  {
    send_to_char( "Mobs can't be in clans.\n\r", ch );
    return;
  }
  if( IS_NPC( ch ) && channel == CHANNEL_ORDER )
  {
    send_to_char( "Mobs can't be in orders.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && channel == CHANNEL_COUNCIL )
  {
    send_to_char( "Mobs can't be in councils.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && channel == CHANNEL_GUILD )
  {
    send_to_char( "Mobs can't be in guilds.\n\r", ch );
    return;
  }

  if( !IS_PKILL( ch ) && channel == CHANNEL_WARTALK )
  {
    if( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "Peacefuls have no need to use wartalk.\n\r", ch );
      return;
    }
  }

  if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
  {
    send_to_char( "You can't do that here.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    if( ch->master )
      send_to_char( "I don't think so...\n\r", ch->master );
    return;
  }

  if( argument[0] == '\0' )
  {
    if( channel == CHANNEL_YELL )
      sprintf( buf, "Area what?\n\r" );
    else
      sprintf( buf, "%s what?\n\r", verb );
    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );  /* where'd this line go? */
    return;
  }

  if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_SILENCE ) )
  {
    ch_printf( ch, "You can't %s.\n\r", verb );
    return;
  }

  if( xIS_SET( ch->deaf, CHANNEL_FOS ) && channel == CHANNEL_FOS )
  {
    send_to_char( "&RThe 'Freedom of Speech' channel is &WNOT &Rmoderated by administrators.\n\r", ch );
    send_to_char( "&RThis channel may contain vulgar language, obscene gestures, adult situations,\n\r", ch );
    send_to_char( "&Rfull frontal nudity, and graphic violence.  Then again...it may not.\n\r", ch );
    send_to_char( "&RYou &WMUST &Rbe at least 18 years of age to use or have this channel active.\n\r", ch );
    send_to_char( "&RIf you are not 18, please type '&Wchannels -fos&R' to turn off this channel.&w\n\r", ch );
  }
  xREMOVE_BIT( ch->deaf, channel );

  if( ch->desc )
    ch->desc->psuppress_channel++;
  switch ( channel )
  {
    default:
      pager_printf_color( ch, "&D&W[&C%s&W] &C%s&W:&C '%s&D&C'&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&W[&C%s&W] &C%s&W:&C '%s&D&C'&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&W[&C%s&W] &CSomeone&W:&C '%s&D&C'&D\n\r", verb, argument );
      break;
      // Think Channel stupid SMAUG people calling it "high" -Goku 09.05.03
    case CHANNEL_HIGH:
      pager_printf_color( ch, "&D&g[&c%s&g]&R%s&w> &Y%s&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&g[&c%s&g]&R%s&w> &Y%s&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&g[&c%s&g]&RSomeone&w> &Y%s&D\n\r", verb, argument );
      break;
      // MUSE Channel stupid SMAUG people calling it "highgod" -Goku 09.05.03
    case CHANNEL_HIGHGOD:
      pager_printf_color( ch, "&D&c%s&G[&g%s&G]&w> &Y%s&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&c%s&G[&g%s&G]&w> &Y%s&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&c%s&G[&gSomeone&G]&w> &Y%s&D\n\r", verb, argument );
      break;
    case CHANNEL_NEWBIE:
      //argument = remove_color(argument);
      pager_printf_color( ch, "&D&C[&R%s&C] &w%s&W:&B '&W%s&D&B'&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&C[&R%s&C] &w%s&W:&B '&W%s&D&B'&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&C[&R%s&C] &wSomeone&W:&B '&W%s&D&B'&D\n\r", verb, argument );
      break;
    case CHANNEL_MUSIC:
      pager_printf_color( ch, "&D&W[&C%s&W] &C%s&W:&c '%s&D&c'&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&W[&C%s&W] &C%s&W:&c '%s&D&c'&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&W[&C%s&W] &CSomeone&W:&c '%s&D&c'&D\n\r", verb, argument );
      break;
    case CHANNEL_ASK:
      //argument = remove_color(argument);
      pager_printf_color( ch, "&D&W[&c%s&W] &C%s&W&W: &z--&W '&O%s&W'&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&W[&c%s&W] &C%s&W&W: &z--&W '&O%s&W'&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&W[&c%s&W] &CSomeone&W&W: &z--&W '&O%s&W'&D\n\r", verb, argument );
      break;
    case CHANNEL_ADMIN:
      //argument = remove_color(argument);
      pager_printf_color( ch, "&D&R[&W%s&R] &R%s&W: &z--&W '%s'&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&R[&W%s&R] &R%s&W: &z--&W '%s'&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&R[&W%s&R] &RSomeone&W: &z--&W '%s'&D\n\r", verb, argument );
      break;
    case CHANNEL_ADMTO:
      argument = one_argument( argument, admName );
      if( ( admVict = get_char_world( ch, admName ) ) == NULL )
      {
        return;
      }
      //argument = remove_color(argument);
      pager_printf_color( ch, "&D&R[&W%s-%s&R] &R%s&W: &z--&W '%s'&D\n\r", verb, admVict->name, ch->name, argument );
      sprintf( buf, "&D&R[&W%s-%s&R] &R%s&W: &z--&W '%s'&D\n\r", verb, admVict->name, ch->name, argument );
      sprintf( buf_invis, "&D&R[&W%s-%s&R] &RSomeone&W: &z--&W '%s'&D\n\r", verb, admVict->name, argument );
      admVict->pcdata->admintalk = 0;
      break;
    case CHANNEL_CLAN:
      pager_printf_color( ch, "&D&R<&z%s&R> &w%s&r: &r--&W '%s&D&W'&D\n\r", ch->pcdata->clan->badge, ch->name, argument );
      sprintf( buf, "&D&R<&z%s&R> &w%s&r: &r--&W '%s&D&W'&D\n\r", ch->pcdata->clan->badge, ch->name, argument );
      sprintf( buf_invis, "&D&R<&z%s&R> &wSomeone&r: &r--&W '%s&D&W'&D\n\r", ch->pcdata->clan->badge, argument );
      break;
    case CHANNEL_SHOUT:
    case CHANNEL_YELL:
      /*
       * argument = remove_color(argument);
       * pager_printf_color( ch, "&D&YYou %s '%s'&D\n\r", verb, argument );
       * sprintf( buf, "&D&Y%s %ss '%s'&D\n\r", ch->name, verb, argument );
       * sprintf( buf_invis, "&D&YSomeone %ss '%s'&D\n\r", verb, argument );
       * break;
       */
      switch ( *argument )
      {
        case '*':
          if( !IS_NPC( ch ) )
          {
            pager_printf_color( ch, "&D&W[&zArea&W] &w%s &c%s*&D\n\r", ch->name, argument );
            sprintf( buf, "&D&W[&zArea&W] &w%s &c%s*&D\n\r", ch->name, argument );
          }
          else
          {
            pager_printf_color( ch, "&D&W[&zArea&W] &w%s &c%s*&D\n\r", ch->short_descr, argument );
            sprintf( buf, "&D&W[&zArea&W] &w%s &c%s*&D\n\r", ch->short_descr, argument );
          }
          sprintf( buf_invis, "&D&W[&zArea&W] &wSomeone &c%s*&D\n\r", argument );
          break;
        default:
          if( !IS_NPC( ch ) )
          {
            pager_printf_color( ch, "&D&W[&zArea&W] &w%s&W: &w--&W '%s'&D\n\r", ch->name, argument );
            sprintf( buf, "&D&W[&zArea&W] &w%s&W: &w--&W '%s'&D\n\r", ch->name, argument );
          }
          else
          {
            pager_printf_color( ch, "&D&W[&zArea&W] &w%s&W: &w--&W '%s'&D\n\r", ch->short_descr, argument );
            sprintf( buf, "&D&W[&zArea&W] &w%s&W: &w--&W '%s'&D\n\r", ch->short_descr, argument );
          }
          sprintf( buf_invis, "&D&W[&zArea&W] &wSomeone&W: &w--&W '%s'&D\n\r", argument );
          break;
      }
      break;
    case CHANNEL_CHAT:
      if( !long_str_prefix( argument, "*dice " ) )
      {
        argument = strip_char_prefix( argument, 6 );
        one_argument( argument, arg1 );
        if( arg1[0] == '\0' )
        {
          send_to_char( "You must provide a number in the range 1 to 100.\n\r", ch );
          break;
        }
        /*
         * if (!IS_NUMBER(arg1))
         * {
         * send_to_char("You must provide a number.\n\r",ch);
         * break;
         * }
         */

        /*
         * if (arg1 < 1 || arg1 > 100)
         * {
         * send_to_char("You must provide a number in the range 1 to 100.\n\r", ch);
         * break;
         * }
         */

        if( arg1[256] < 1 )
        {
          send_to_char( "You must provide a number in the range 1 to 100.\n\r", ch );
          break;
        }
        dinum = atoi( arg1 );
        {
          dival = number_range( 1, dinum );
          pager_printf_color( ch, "&D&w[&z%s:&wDICE&w] &O%s: &w-- 'rolls a %d/%d.'&D\n\r", verb, ch->name, dival, dinum );
          sprintf( buf, "&D&w[&c%s:&zDICE&w] &O%s: &w-- 'rolls a %d/%d.'&D\n\r", verb, ch->name, dival, dinum );
          sprintf( buf_invis, "&D&w[&c%s:&zDICE&w] &OSomeone: &w-- 'rolls a %d/%d.'&D\n\r", verb, dival, dinum );
        }
      }
      else if( !long_str_prefix( argument, "*" ) )
      {
        argument = strip_char_prefix( argument, 1 );
        pager_printf_color( ch, "&D&W[&Y%s&W] &g%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&W[&Y%s&W] &g%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&W[&Y%s&W] &gSomeone %s&D\n\r", verb, argument );
      }
      else if( !long_str_prefix( argument, "ooc *" ) )
      {
        channel = CHANNEL_CHAT_OOC;
        argument = strip_char_prefix( argument, 5 );
        pager_printf_color( ch, "&D&B[&c%s:&zOOC&B] &c%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&B[&c%s:&zOOC&B] &c%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&B[&c%s:&zOOC&B] &cSomeone %s&D\n\r", verb, argument );
      }
      else if( !long_str_prefix( argument, "ooc " ) )
      {
        channel = CHANNEL_CHAT_OOC;
        argument = strip_char_prefix( argument, 4 );
        pager_printf_color( ch, "&D&B[&c%s:&zOOC&B] &O%s: &w-- &g'%s'&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&B[&c%s:&zOOC&B] &O%s: &w-- &g'%s'&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&B[&c%s:&zOOC&B] &OSomeone: &w-- &g'%s'&D\n\r", verb, argument );
      }
      else
      {
        pager_printf_color( ch, "&D&W[&Y%s&W] &G%s: &w-- &W'%s'&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&W[&Y%s&W] &G%s: &w-- &W'%s'&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&W[&Y%s&W] &GSomeone: &w-- &W'%s'&D\n\r", verb, argument );
      }
      break;

/*			switch ( *argument )
			{
				case '*':
					pager_printf_color( ch, "&D&W[&Y%s&W] &G%s &g%s*&D\n\r", verb, ch->name, argument );
					sprintf( buf, "&D&W[&Y%s&W] &G%s &g%s*&D\n\r", verb, ch->name, argument );
					sprintf( buf_invis, "&D&W[&Y%s&W] &GSomeone &g%s*&D\n\r", verb, argument );
					break;
				default:
					pager_printf_color( ch, "&D&W[&Y%s&W] &G%s&W: &z--&W '%s&D&W'&D\n\r", verb, ch->name, argument );
					sprintf( buf, "&D&W[&Y%s&W] &G%s&W: &z--&W '%s&D&W'&D\n\r", verb, ch->name, argument );
					sprintf( buf_invis, "&D&W[&Y%s&W] &GSomeone&W: &z--&W '%s&D&W'&D\n\r", verb, argument );
					break;
			}
			break;
*/
    case CHANNEL_HC:
      switch ( *argument )
      {
        case '*':
          pager_printf_color( ch, "&D&R[&p%s&R] &c%s &c%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&R[&p%s&R] &c%s &c%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&P[&p%s&P] &cSomeone &c%s*&D\n\r", verb, argument );
          break;
        default:
          pager_printf_color( ch, "&D&R[&p%s&R] &c%s&W: &z-- &W'&z%s&D&W'&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&R[&p%s&R] &c%s&W: &z-- &W'&z%s&D&W'&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&R[&p%s&R] &cSomeone&W: &z-- &W'&z%s&D&W'&D\n\r", verb, argument );
          break;
      }
      break;
    case CHANNEL_OOC:
      switch ( *argument )
      {
        case '*':
          if( !IS_NPC( ch ) )
          {
            pager_printf_color( ch, "&D&B[&z%s&B] &C%s &c%s*&D\n\r", verb, ch->name, argument );
            sprintf( buf, "&D&B[&z%s&B] &C%s &c%s*&D\n\r", verb, ch->name, argument );
          }
          else
          {
            pager_printf_color( ch, "&D&B[&z%s&B] &C%s &c%s*&D\n\r", verb, ch->short_descr, argument );
            sprintf( buf, "&D&W[&B%s&W] &C%s &c%s*&D\n\r", verb, ch->short_descr, argument );
          }
          sprintf( buf_notag, "&D&B[&Z%s&B] &C%s &c%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&B[&z%s&B] &CSomeone &c%s*&D\n\r", verb, argument );
          break;
        default:
          if( IS_NPC( ch ) )
          {
            sprintf( buf, "&D&B[&z%s&B] &z{&GNPC&z} &C%s &c%s&D\n\r", verb, ch->name, argument );
          }
          else
          {
            sprintf( buf, "&D&B[&z%s&B] &z{&G%s&z} &C%s&W: &z-- &W'%s&D&W'&D\n\r", verb, colored_race_name( ch ), ch->name, argument );
          }
          sprintf( buf_notag, "&D&B[&z%s&B] &C%s&W: &z-- &W'%s&D&W'&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&B[&z%s&B] &C%s&W: &z-- &W'%s&D&W'&D\n\r", verb, "Someone", argument );
          if( xIS_SET( ch->act, PLR_MAKE_THESE_STUPID_CHANNEL_TAGS_GO_AWAY ) )
		  {
            send_to_pager_color( buf_notag, ch );
		  }
          else
		  {
            send_to_pager_color( buf, ch );
		  }

            break;
      }
      break;
    case CHANNEL_FOS:
      switch ( *argument )
      {
        case '*':
          pager_printf_color( ch, "&D&W[&O%s&W] &O%s &w%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&W[&O%s&W] &O%s &w%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&W[&O%s&W] &OSomeone &w%s*&D\n\r", verb, argument );
          break;
        default:
          pager_printf_color( ch, "&D&W[&O%s&W] &O%s&W: &z-- &w'%s&D&w'&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&W[&O%s&W] &O%s&W: &z-- &w'%s&D&w'&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&W[&O%s&W] &OSomeone&W: &z-- &w'%s&D&w'&D\n\r", verb, argument );
          break;
      }
      break;
    case CHANNEL_RACETALK:
      pager_printf_color( ch, "&D&W(&g%s&w&W) &G%s: &z--&W '%s&D&W'&D\n\r", verb, ch->name, argument );
      sprintf( buf, "&D&W(&g%s&w&W) &G%s: &z--&W '%s&D&W'&D\n\r", verb, ch->name, argument );
      sprintf( buf_invis, "&D&W(&g%s&w&W) &GSomeone: &z--&W '%s&D&W'&D\n\r", verb, argument );
      break;
    case CHANNEL_TRAFFIC:
      pager_printf_color( ch, "&D&cYou %s:  %s&D\n\r", verb, argument );
      sprintf( buf, "&D&c%s %ss '%s'&D\n\r", ch->name, verb, argument );
      sprintf( buf_invis, "&D&cSomeone %ss '%s'&D\n\r", verb, argument );
      break;
    case CHANNEL_WARTALK:
      switch ( *argument )
      {
        case '*':
          pager_printf_color( ch, "&D&W[&R%s&W] &O%s &O%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&W[&R%s&W] &O%s &O%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&W[&R%s&W] &OSomeone &O%s*&D\n\r", verb, argument );
          break;
        default:
          pager_printf_color( ch, "&D&W[&R%s&W] &O%s: &z--&W '%s&D&W'&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&W[&R%s&W] &O%s: &z--&W '%s&D&W'&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&W[&R%s&W] &OSomeone: &z--&W '%s&D&W'&D\n\r", verb, argument );
          break;
      }
      break;
    case CHANNEL_IMMTALK:
    case CHANNEL_AVTALK:
      sprintf( buf, "&R -&z%s&w%c&W %s&D\n\r", ch->name, channel == CHANNEL_IMMTALK ? '>' : ':', argument );
      sprintf( buf_invis, "&R -&zSomeone&w%c&W %s&D\n\r", channel == CHANNEL_IMMTALK ? '>' : ':', argument );
      position = ch->position;
      ch->position = POS_STANDING;
      send_to_pager_color( buf, ch );
      //  pager_printf_color (ch, buf);
      //    act( AT_IMMORT, buf, ch, argument, NULL, TO_CHAR );
      ch->position = position;
      break;
    case CHANNEL_ROLEPLAY:
      if( !long_str_prefix( argument, "*dice " ) )
      {
        argument = strip_char_prefix( argument, 6 );
        one_argument( argument, arg1 );
        if( arg1[0] == '\0' )
        {
          send_to_char( "You must provide a di number in the range 1 to 20.\n\r", ch );
          break;
        }
        dinum = atoi( arg1 );
        {
          dival = number_range( 1, dinum );
          pager_printf_color( ch, "&D&W[&g%s&W:&gOOC&W] &W%s rolled 1d%d and got %d.&D\n\r", verb, ch->name, dinum, dival );
          sprintf( buf, "&D&W[&g%s&W:&gOOC&W] &W%s rolled 1d%d and got %d.&D\n\r", verb, ch->name, dinum, dival );
          sprintf( buf_invis, "&D&W[&g%s&W:&gOOC&W] &WSomeone rolled 1d%d and got %d.&D\n\r", verb, dinum, dival );
        }
      }
      else if( !long_str_prefix( argument, "*" ) )
      {
        argument = strip_char_prefix( argument, 1 );
        pager_printf_color( ch, "&D&W[&g%s&W] &O%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&W[&g%s&W] &O%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&W[&g%s&W] &OSomeone %s&D\n\r", verb, argument );
      }

      else if( !long_str_prefix( argument, "ooc *" ) )
      {
        argument = strip_char_prefix( argument, 5 );
        pager_printf_color( ch, "&D&W[&g%s&W:&gOOC&W] &w%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&W[&g%s&W:&gOOC&W] &w%s %s&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&W[&g%s&W:&gOOC&W] &wSomeone %s&D\n\r", verb, argument );
      }
      else if( !long_str_prefix( argument, "ooc " ) )
      {
        argument = strip_char_prefix( argument, 4 );
        pager_printf_color( ch, "&D&W[&g%s&W:&gOOC&W] &O%s&W: &W'&w%s&W'&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&W[&g%s&W:&gOOC&W] &O%s&W: &W'&w%s&W'&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&W[&g%s&W:&gOOC&W] &OSomeone&W: &W'&w%s&W'&D\n\r", verb, argument );
      }
      else
      {
        pager_printf_color( ch, "&D&W[&g%s&W] &O%s&W: '&G%s&W'&D\n\r", verb, ch->name, argument );
        sprintf( buf, "&D&W[&g%s&W] &O%s&W: '&G%s&W'&D\n\r", verb, ch->name, argument );
        sprintf( buf_invis, "&D&W[&g%s&W] &OSomeone&W: '&G%s&W'&D\n\r", verb, argument );
      }
      break;
    case CHANNEL_COUNCIL:
      switch ( *argument )
      {
        case '*':
          pager_printf_color( ch, "&D&Y[&z%s&Y] %s &z%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&Y[&z%s&Y] %s &z%s*&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&Y[&z%s&Y] Someone &z%s*&D\n\r", verb, argument );
          break;
        default:
          pager_printf_color( ch, "&D&Y[&z%s&Y] %s&W: '&z%s&D&W'&D\n\r", verb, ch->name, argument );
          sprintf( buf, "&D&Y[&z%s&Y] %s&W: '&z%s&D&W'&D\n\r", verb, ch->name, argument );
          sprintf( buf_invis, "&D&Y[&z%s&Y] Someone&W: '&z%s&D&W'&D\n\r", verb, argument );
          break;
      }
      break;
  }

  if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
  {
    sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
    append_to_file( LOG_FILE, buf2 );
  }


#ifdef HMM
  if( is_profane( argument ) )
  {
    sprintf( buf2, "%s Profanity warning: %s: %s (%s)", "say", IS_NPC( ch ) ? ch->short_descr : ch->name, argument, verb );
    /*
     * force Puff mpat 6 mpforce imp mpat 1 say hi 
     */

    puff = get_char_world( ch, "Puff" );
    if( puff != NULL )
    {
      if( ( location = get_room_index( 1 ) ) != NULL )
      {
        original = puff->in_room;
        char_from_room( puff );
        char_to_room( puff, location );
        interpret( puff, buf2 );
        char_to_room( puff, original );
      }
    }
  }
#endif

  for( d = first_descriptor; d; d = d->next )
  {
    CHAR_DATA *och;
    CHAR_DATA *vch;
	char *sbuf = argument;

    och = d->original ? d->original : d->character;
    vch = d->character;

    if( ( d->connected == CON_PLAYING ||
          ( d->connected == CON_EDITING && xIS_SET( och->act, PLR_CHANEDIT ) ) )
        && vch != ch && !xIS_SET( och->deaf, channel ) )
    {
      char lbuf[MAX_INPUT_LENGTH + 4];  /* invis level string + buf */

      /*
       * fix by Gorog os that players can ignore others' channel talk 
       */
      /*
       * fix so you can ignore anyone other than an Imm -- Melora 
       */
      if( is_ignoring( och, ch ) && !IS_IMMORTAL( och ) /*get_trust(ch) <= get_trust(och) */  )
        continue;

      /*
       * Adding newbie in for all
       * 
       * if ( channel != CHANNEL_NEWBIE && NOT_AUTHED(och) )
       * continue;
       */
      if( channel == CHANNEL_IMMTALK && !IS_IMMORTAL( och ) )
        continue;
      /*
       * if ( channel == CHANNEL_ADMIN && !IS_IMMORTAL(och) )
       * continue;
       */
      if( channel == CHANNEL_HC && ( !IS_HC( och ) && !IS_IMMORTAL( och ) ) )
        continue;
      if( channel == CHANNEL_WARTALK && NOT_AUTHED( och ) )
        continue;
      if( channel == CHANNEL_AVTALK && !IS_HERO( och ) )
        continue;
      if( channel == CHANNEL_HIGHGOD && get_trust( och ) < sysdata.muse_level )
        continue;
      if( channel == CHANNEL_HIGH && get_trust( och ) < sysdata.think_level )
        continue;

      /*
       * Make it so only admin see the admin chanel -- Melora 
       */
      if( IS_IMMORTAL( ch ) )
      {
        ;
      }
      else
      {
        if( channel == CHANNEL_ADMIN && !IS_IMMORTAL( och ) )
          continue;
      }

      /*
       * Only imms and the victim can see adminto -- Melora 
       */
      if( channel == CHANNEL_ADMTO && ( !IS_IMMORTAL( och ) && ( och != admVict ) ) )
        continue;

      if( channel == CHANNEL_TRAFFIC && !IS_IMMORTAL( och ) && !IS_IMMORTAL( ch ) )
      {
        if( ( IS_HERO( ch ) && !IS_HERO( och ) ) || ( !IS_HERO( ch ) && IS_HERO( och ) ) )
          continue;
      }

      /*
       * Fix by Narn to let newbie council members see the newbie channel. 
       */
      /*
       * if ( channel == CHANNEL_NEWBIE  &&
       * ( !IS_IMMORTAL(och) && !NOT_AUTHED(och)
       * && !( och->pcdata->council &&
       * !str_cmp( och->pcdata->council->name, "Newbie Council" ) ) ) )
       * continue;
       */
      if( xIS_SET( vch->in_room->room_flags, ROOM_SILENCE ) )
        continue;
      if( channel == CHANNEL_YELL && vch->in_room->area != ch->in_room->area )
        continue;

      if( channel == CHANNEL_CLAN || channel == CHANNEL_ORDER || channel == CHANNEL_GUILD )
      {
        if( IS_NPC( vch ) )
          continue;
        if( vch->pcdata->clan != ch->pcdata->clan )
          continue;
        if( is_swear( argument ) && !xIS_SET( vch->act, PLR_CLAN_SWEAR ) )
        {
          pager_printf_color( vch, "&W(&CCLAN&W) &c%s starts swearing like there's no tomorrow!\n\r", ch->name );
          continue;
        }
      }
      if( channel == CHANNEL_OOC )
      {
        if( IS_NPC( vch ) )
          continue;
        if( unread_notes( ch, &boards[0] ) > 0 && !IS_IMMORTAL( ch ) && !IS_NPC(ch) )
        {
          send_to_char( "\n\r&YYour last message did not go through. Please follow the instructions below.\n\r", ch );
          send_to_char
            ( "&gPlease read all of the notes on the announcement board first.\n\r&zSyntax:&W gboard 1\n\r        &Wgnote read\n\r",
              ch );
          return;
        }
        if( is_swear( argument ) && !xIS_SET( vch->act, PLR_CLAN_SWEAR ) )
        {
          pager_printf_color( vch, "&W[&BOOC&W] &C%s &c*starts swearing*\n\r", ch->name );
          continue;
        }
      }

      if( channel == CHANNEL_COUNCIL )
      {
        if( IS_NPC( vch ) )
          continue;
        if( vch->pcdata->council != ch->pcdata->council )
          continue;
      }


      if( channel == CHANNEL_RACETALK )
      {
        if( vch->race != ch->race )
        {
          if( is_android( ch ) && !is_android( vch ) )
            continue;
          else if( !is_android( ch ) && is_android( vch ) )
            continue;
          else if( is_saiyan( ch ) && !is_saiyan( vch ) )
            continue;
          else if( !is_saiyan( ch ) && is_saiyan( vch ) )
            continue;
          else if( IS_DEMON( ch ) && !IS_DEMON( vch ) )
            continue;
          else if( !IS_DEMON( ch ) && IS_DEMON( vch ) )
            continue;
          else if( IS_KAIO( ch ) && !IS_KAIO( vch ) )
            continue;
          else if( !IS_KAIO( ch ) && IS_KAIO( vch ) )
            continue;
          else if( is_human( ch ) && !is_human( vch ) )
            continue;
          else if( !is_human( ch ) && is_human( vch ) )
            continue;
          else if( is_namek( ch ) && !is_namek( vch ) )
            continue;
          else if( !is_namek( ch ) && is_namek( vch ) )
            continue;
          else if( is_bio( ch ) && !is_bio( vch ) )
            continue;
          else if( !is_bio( ch ) && is_bio( vch ) )
            continue;
          else if( IS_ICER( ch ) && !IS_ICER( vch ) )
            continue;
          else if( !IS_ICER( ch ) && IS_ICER( vch ) )
            continue;
          else if( is_hb( ch ) && !is_hb( vch ) )
            continue;
          else if( !is_hb( ch ) && is_hb( vch ) )
            continue;
          else if( is_majin( ch ) && !is_majin( vch ) )
            continue;
          else if( !is_majin( ch ) && is_majin( vch ) )
            continue;
          else if( is_genie( ch ) && !is_genie( vch ) )
            continue;
          else if( !is_genie( ch ) && is_genie( vch ) )
            continue;
          else if( is_saibaman( ch ) && !is_saibaman( vch ) )
            continue;
          else if( !is_saibaman( ch ) && is_saibaman( vch ) )
            continue;
          else if( is_tuffle( ch ) && !is_tuffle( vch ) )
            continue;
          else if( !is_tuffle( ch ) && is_tuffle( vch ) )
            continue;
          else if( is_yardratian( ch ) && !is_yardratian( vch ) )
            continue;
          else if( !is_yardratian( ch ) && is_yardratian( vch ) )
            continue;
          else if( is_symbiote( ch ) && !is_symbiote( vch ) )
            continue;
          else if( !is_symbiote( ch ) && is_symbiote( vch ) )
            continue;
		  else if( !is_hylian( ch ) && is_hylian( vch ) )
            continue;
		  else if( !is_fierian( ch ) && is_fierian( vch ) )
            continue;
		  else if( !is_reploid( ch ) && is_reploid( vch ) )
            continue;
        }
      }

      if( xIS_SET( ch->act, PLR_WIZINVIS ) && can_see( vch, ch ) && IS_IMMORTAL( vch ) )
      {
        sprintf( lbuf, "(%d) ", ( !IS_NPC( ch ) ) ? ch->pcdata->wizinvis : ch->mobinvis );
      }
      else
      {
        lbuf[0] = '\0';
      }

      position = vch->position;
      if( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
        vch->position = POS_STANDING;
#ifndef SCRAMBLE

      if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
      {
        int speakswell = UMIN( knows_language( vch, ch->speaking, ch ),
                               knows_language( ch, ch->speaking, vch ) );

        if( speakswell < 85 )
		{
          sbuf = translate( speakswell, argument, lang_names[speaking] );
		}
      }
#else
      if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) || ch->speaking != 0 ) )
        sbuf = scramble( argument, ch->speaking );
#endif
      /*
       * Scramble speech if vch or ch has nuisance flag 
       */

      if( !IS_NPC( ch ) && ch->pcdata->nuisance
          && ch->pcdata->nuisance->flags > 7
          && ( number_percent(  ) < ( ( ch->pcdata->nuisance->flags - 7 ) * 10 * ch->pcdata->nuisance->power ) ) )
        sbuf = scramble( argument, number_range( 1, 10 ) );

      if( !IS_NPC( vch ) && vch->pcdata->nuisance &&
          vch->pcdata->nuisance->flags > 7
          && ( number_percent(  ) < ( ( vch->pcdata->nuisance->flags - 7 ) * 10 * vch->pcdata->nuisance->power ) ) )
        sbuf = scramble( argument, number_range( 1, 10 ) );

      MOBtrigger = FALSE;

      if( vch->desc )
        vch->desc->psuppress_channel++;

      if( can_see( vch, ch ) )
      {
        if( !xIS_SET( ch->act, PLR_NOSCREAM ) || channel == CHANNEL_CHAT || channel == CHANNEL_CHAT_OOC )
        {
          if( channel == CHANNEL_OOC && xIS_SET( vch->act, PLR_MAKE_THESE_STUPID_CHANNEL_TAGS_GO_AWAY ) )
            send_to_pager_color( strcat( lbuf, buf_notag ), vch );
          else
            send_to_pager_color( strcat( lbuf, buf ), vch );
        }
      }
      else
      {
        if( !xIS_SET( ch->act, PLR_NOSCREAM ) || channel == CHANNEL_CHAT || channel == CHANNEL_CHAT_OOC )
          send_to_pager_color( strcat( lbuf, buf_invis ), vch );
      }
      vch->position = position;
    }
  }

  /*
   * too much system degradation with 300+ players not to charge 'em a bit 
   */
  /*
   * 600 players now, but waitstate on clantalk is bad for pkillers 
   */
  if( ch->level < 51 )
    WAIT_STATE( ch, 6 );

  return;
}

void to_channel( const char *argument, int channel, const char *verb, sh_int level )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  if( !first_descriptor || argument[0] == '\0' )
    return;

  sprintf( buf, "%s: %s&w\r\n", verb, argument );

  for( d = first_descriptor; d; d = d->next )
  {
    CHAR_DATA *och;
    CHAR_DATA *vch;

    och = d->original ? d->original : d->character;
    vch = d->character;

    if( !och || !vch )
      continue;
    if( !IS_IMMORTAL( vch )
        || ( get_trust( vch ) < sysdata.build_level && channel == CHANNEL_BUILD )
        || ( get_trust( vch ) < sysdata.log_level
             && ( channel == CHANNEL_LOG || channel == CHANNEL_HIGH ||
                  channel == CHANNEL_WARN || channel == CHANNEL_COMM ) ) )
      continue;

    if( d->connected == CON_PLAYING && !xIS_SET( och->deaf, channel ) && get_trust( vch ) >= level )
    {
      set_char_color( AT_LOG, vch );
      send_to_char_color( buf, vch );
    }
  }

  return;
}

void do_roleplay( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && !xIS_SET( ch->act, PLR_QUESTING ) )
  {
    send_to_char( "You can't use this channel because you are not part of this role playing event.\n\r", ch );
    return;
  }

  if( !sysdata.rpChannel )
  {
    send_to_char( "The role play channel is currently off.  It will be turned back on when the event resumes.\n\r", ch );
    return;
  }

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_ROLEPLAY, "RolePlay" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_hcchat( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) || ( !IS_HC( ch ) && !IS_IMMORTAL( ch ) ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_HC, "&BH&CA&BR&CD&BC&CO&BR&CE" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_fos( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( xIS_SET( ch->act, PLR_NO_FOS ) )
  {
    send_to_char( "Your Freedom has been denied.\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_FOS, "FOS" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}
void do_admintalk( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) )
  {
    pager_printf_color( ch, "NPC's can use the ADMIN channel." );
    return;
  }

  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( get_trust( ch ) < LEVEL_IMMORTAL && ch->pcdata->admintalk > 0 )
  {
    pager_printf_color( ch, "You must wait %d more minutes before you can ask another question.\n\r",
                        ch->pcdata->admintalk );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }

  /*
   * if (!can_admin(ch))
   * *      {
   * *      send_to_char("&wSorry, no admins are online or they are not answering questions at this time.\n\r", ch);
   * *      send_to_char("&wPlease try to ask your question again at a later time.  Thank you.\n\r", ch);
   * *        return;
   * *      }
   * *      else
   */
  undeaf_admin(  );

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_ADMIN, "ADMIN" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  ch->pcdata->admintalk = 1;
  return;
}

void do_chat( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( !xIS_SET( ch->act, PLR_CAN_CHAT ) && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "You must first have an admin authorize your bio to use this channel.\n\r", ch );
    return;
  }
/*
	if (is_swear(argument))
	{
		send_to_char("Oops, you shouldn't be swearing!\n\r", ch);
		return ;
	}
*/

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_CHAT, "CHAT" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_ooc( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
/*  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }*/

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_OOC, "OOC" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_clantalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( IS_NPC( ch ) || !ch->pcdata->clan
      || ch->pcdata->clan->clan_type == CLAN_ORDER || ch->pcdata->clan->clan_type == CLAN_GUILD )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }

  /*
   * if (is_swear(argument))
   * {
   * send_to_char("Oops, you shouldn't be swearing!\n\r", ch);
   * return;
   * }
   */
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_CLAN, "&zClan" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_newbiechat( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) )
    /*
     * || ( !NOT_AUTHED( ch ) && !IS_IMMORTAL(ch)
     * && !( ch->pcdata->council &&
     * !str_cmp( ch->pcdata->council->name, "Newbie Council" ) ) ) )
     */
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( unread_notes( ch, &boards[0] ) > 0 )
  {
    send_to_char( "\n\r&YYour last message did not go through. Please follow the instructions below.\n\r", ch );
    send_to_char
      ( "&gPlease read all of the notes on the announcement board first.\n\r&zSyntax:&W gboard 1\n\r        &Wgnote read\n\r",
        ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ( has_phrase( ".com", argument )
                         || has_phrase( ".net", argument ) || has_phrase( ".org", argument ) || has_phrase( ".uk", argument )
                         || has_phrase( "dbna", argument ) || has_phrase( "dbnu", argument ) || has_phrase( "www", argument )
                         || has_phrase( ".nu", argument ) ) )
  {
    //sprintf( buf, "%s has just typed \"%s\". Possible advertisement found\n\r", ch->name, argument );
    //log_string( buf );
    //do_deny(ch, "self");
    //talk_channel( ch, drunk_speech( "Hey, I'm a Newbie who likes to use URLs!", ch ), CHANNEL_NEWBIE, "NewbiE" );
    send_to_char( "Oops, you shouldn't be using urls in the Newbie channel.\n\r", ch );
    send_to_char( "You will now be put into a 10 min timeout.\n\r", ch );
    send_to_char( "Now, Think about what you have done.\n\r", ch );
    xSET_BIT( ch->act, PLR_SILENCE );
    ch->pcdata->silence = 10;
    return;
  }
  //argument = remove_color(argument);
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_NEWBIE, "&YN&Re&Yw&Rb&Yi&RE" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_ot( CHAR_DATA * ch, char *argument )
{
  do_ordertalk( ch, argument );
}

void do_ordertalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) || !ch->pcdata->clan || ch->pcdata->clan->clan_type != CLAN_ORDER )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_ORDER, "ORDER" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_counciltalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) || !ch->pcdata->council )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_COUNCIL, "COUNCIL" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_guildtalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) || !ch->pcdata->clan || ch->pcdata->clan->clan_type != CLAN_GUILD )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_GUILD, "GUILD" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_music( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_MUSIC, "&RM&WU&PS&YI&GC&w" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}


void do_games( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_QUEST, "GAMES" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_ask( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
//  argument = remove_color(argument);
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_ASK, "QUESTION" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}



void do_answer( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
//  argument = remove_color(argument);
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_ASK, "QUESTION" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}



void do_shout( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SHOUT, "shout" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  WAIT_STATE( ch, 12 );
  return;
}



void do_yell( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_YELL, "yell" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}



void do_immtalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}


void do_muse( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_HIGHGOD, "MUSE" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}


void do_think( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_HIGH, "THINK" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}


void do_avtalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_AVTALK, "avtalk" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}


void do_say( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *vch;
  CHAR_DATA *robot = NULL;

  EXT_BV actflags;
#ifndef SCRAMBLE

  int speaking = -1, lang;

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif

  if( argument[0] == '\0' )
  {
    send_to_char( "Say what?\n\r", ch );
    return;
  }

  if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
  {
    send_to_char( "You can't do that here.\n\r", ch );
    return;
  }

  /*
   * Bot check stuff since for the "answer" i can't use a mob prog -Goku 09.26.04 
   */
  if( ch->in_room->vnum == 6500 || ch->in_room->vnum == 6510 )
  {
    /*
     * code located in mud_comm.c even though it's no a mob prog 
     */
    mpchkbot( ch, argument );
    return;
  }

  //  argument = remove_color(argument);
  actflags = ch->act;
  if( IS_NPC( ch ) )
    xREMOVE_BIT( ch->act, ACT_SECRETIVE );
  if( is_swear( argument ) && !xIS_SET( ch->act, PLR_SAY_SWEAR ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }

  /*
   * if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_SILENCE) )
   * {
   * send_to_char("You're silenced. You can't speak.\n\r",ch);
   * return ;
   * }
   */

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    interpret( ch, "roar" );
    return;
  }

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
  {
    char *sbuf = argument;

    if( vch == ch )
      continue;

    if( IS_NPC( vch ) && xIS_SET( vch->act, ACT_ROBOT ) )
      robot = vch;

    /*
     * Check to see if character is ignoring speaker 
     */
    if( is_ignoring( vch, ch ) )
    {
      /*
       * continue unless speaker is an immortal 
       */
      if( !IS_IMMORTAL( ch ) || get_trust( vch ) > get_trust( ch ) )
        continue;
      /*
       * else
       * *    {
       * *      set_char_color(AT_IGNORE, vch);
       * *      ch_printf(vch,"You attempt to ignore %s, but"
       * *        " are unable to do so.\n\r", ch->name);
       * *    }
       */
    }

#ifndef SCRAMBLE
    if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
    {
      int speakswell = UMIN( knows_language( vch, ch->speaking, ch ),
                             knows_language( ch, ch->speaking, vch ) );

      if( speakswell < 75 )
        sbuf = translate( speakswell, argument, lang_names[speaking] );
    }
#else
    if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) || ch->speaking != 0 ) )
      sbuf = scramble( argument, ch->speaking );
#endif

    sbuf = drunk_speech( sbuf, ch );

    MOBtrigger = FALSE;
    if( !IS_NPC( vch ) && is_swear( argument ) && !xIS_SET( vch->act, PLR_SAY_SWEAR ) )
      act( AT_SAY, "$n starts swearing up a storm.", ch, sbuf, vch, TO_VICT );
    else if( !xIS_SET( ch->act, PLR_NOSCREAM ) )
    {
      if( is_fused( ch ) && !is_superandroid( ch ) && !is_namek( ch ) )
        act( AT_SAY, "$n says in a dual voice '$t'", ch, sbuf, vch, TO_VICT );
      else
        act( AT_SAY, "$n says '$t'", ch, sbuf, vch, TO_VICT );
    }
  }
  /*
   * MOBtrigger = FALSE;
   * act( AT_SAY, "$n says '$T'", ch, NULL, argument, TO_ROOM );
   */
  ch->act = actflags;
  MOBtrigger = FALSE;
  if( is_fused( ch ) && !is_superandroid( ch ) && !is_namek( ch ) )
    act( AT_SAY, "\n\rYou say in a dual voice '$T'", ch, NULL, drunk_speech( argument, ch ), TO_CHAR );
  else
    act( AT_SAY, "\n\rYou say '$T'", ch, NULL, drunk_speech( argument, ch ), TO_CHAR );
  if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
  {
    sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
    append_to_file( LOG_FILE, buf );
  }
  if( robot != NULL )
    if( IS_NPC( robot ) && xIS_SET( robot->act, ACT_ROBOT ) )
    {
      chatperform( robot, ch, argument, 1 );
    }
  sysdata.outBytesFlag = LOGBOUTNORM;
  mprog_speech_trigger( argument, ch );
  if( char_died( ch ) )
    return;
  oprog_speech_trigger( argument, ch );
  if( char_died( ch ) )
    return;
  rprog_speech_trigger( argument, ch );
  return;
}


void do_whisper( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int position;
  int speaking = -1, lang;
#ifndef SCRAMBLE

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif

  xREMOVE_BIT( ch->deaf, CHANNEL_WHISPER );

  argument = one_argument( argument, arg );

  if( arg[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "Whisper to whom what?\n\r", ch );
    return;
  }


  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( ch == victim )
  {
    send_to_char( "You have a nice little chat with yourself.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && ( victim->switched ) && !IS_AFFECTED( victim->switched, AFF_POSSESS ) )
  {
    send_to_char( "That player is switched.\n\r", ch );
    return;
  }
  else if( !IS_NPC( victim ) && ( !victim->desc ) )
  {
    send_to_char( "That player is link-dead.\n\r", ch );
    return;
  }
  if( !IS_NPC( victim ) )
    if( xIS_SET( victim->act, PLR_AFK ) )
    {
      send_to_char( "That player is afk.\n\r", ch );
      return;
    }
  if( xIS_SET( victim->deaf, CHANNEL_WHISPER ) && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
  {
    act( AT_PLAIN, "$E has $S whispers turned off.", ch, NULL, victim, TO_CHAR );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    ch_printf( ch, "You can't while Oozaru.\n\r" );
    return;
  }
  if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_SILENCE ) )
    send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

  if( victim->desc  /* make sure desc exists first  -Thoric */
      && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_GOD )
  {
    act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
    return;
  }

  /*
   * Check to see if target of tell is ignoring the sender 
   */
  if( is_ignoring( victim, ch ) )
  {
    /*
     * If the sender is an imm then they cannot be ignored 
     */
    if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
    {
      set_char_color( AT_IGNORE, ch );
      ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
      return;
    }
    /*
     * else
     * *  {
     * *    set_char_color(AT_IGNORE, victim);
     * *    ch_printf(victim, "You attempt to ignore %s, but "
     * *      "are unable to do so.\n\r", ch->name);
     * *  }
     */
  }

  /*
   * Bug fix by guppy@wavecomputers.net 
   */
  MOBtrigger = FALSE;

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  act( AT_WHISPER, "You whisper to $N '$t'", ch, argument, victim, TO_CHAR );
  position = victim->position;
  victim->position = POS_STANDING;
#ifndef SCRAMBLE

  if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
  {
    int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                           knows_language( ch, ch->speaking, victim ) );

    if( speakswell < 85 )
      act( AT_WHISPER, "$n whispers to you '$t'", ch,
           translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
#else

  if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) || ch->speaking != 0 ) )
    act( AT_WHISPER, "$n whispers to you '$t'", ch,
         translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
#endif

  else
    act( AT_WHISPER, "$n whispers to you '$t'", ch, argument, victim, TO_VICT );
}

else
act( AT_WHISPER, "$n whispers to you '$t'", ch, argument, victim, TO_VICT );

if( !xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
  act( AT_WHISPER, "$n whispers something to $N.", ch, argument, victim, TO_NOTVICT );

MOBtrigger = TRUE;

victim->position = position;
if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
{
  sprintf( buf, "%s: %s (whisper to) %s.",
           IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
  append_to_file( LOG_FILE, buf );
}

sysdata.outBytesFlag = LOGBOUTNORM;
mprog_speech_trigger( argument, ch );
return;
}

void do_tell( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int position;
  CHAR_DATA *switched_victim = NULL;
#ifndef SCRAMBLE

  int speaking = -1, lang;

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif

  xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
  if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
  {
    send_to_char( "You can't do that here.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ( xIS_SET( ch->act, PLR_SILENCE ) || xIS_SET( ch->act, PLR_NO_TELL ) ) )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );

  if( arg[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "Tell whom what?\n\r", ch );
    return;
  }

  // Typing 0.krill is annoying as HELL
  if( ( victim = get_char_room( ch, arg ) ) == NULL )
    victim = get_pc_world( ch, arg );

  if( victim == NULL
      || ( IS_NPC( victim ) && victim->in_room != ch->in_room )
      || ( !NOT_AUTHED( ch ) && NOT_AUTHED( victim ) && !IS_IMMORTAL( ch ) ) )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( ch == victim )
  {
    send_to_char( "You have a nice little chat with yourself.\n\r", ch );
    return;
  }

  /*
   * if (NOT_AUTHED(ch) && !NOT_AUTHED(victim) && !IS_IMMORTAL(victim) )
   * {
   * send_to_char( "They can't hear you because you are not authorized.\n\r", ch);
   * return;
   * }
   */
  if( !IS_NPC( victim ) && ( victim->switched )
      && ( get_trust( ch ) > LEVEL_AVATAR ) && !IS_AFFECTED( victim->switched, AFF_POSSESS ) )
  {
    send_to_char( "That player is switched.\n\r", ch );
    return;
  }

  else if( !IS_NPC( victim ) && ( victim->switched ) && IS_AFFECTED( victim->switched, AFF_POSSESS ) )
    switched_victim = victim->switched;

  else if( !IS_NPC( victim ) && ( !victim->desc ) )
  {
    send_to_char( "That player is link-dead.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) )
    if( xIS_SET( victim->act, PLR_AFK ) )
    {
      send_to_char( "That player is afk.\n\r", ch );
      return;
    }
  if( !IS_NPC( victim ) )
    if( IS_SET( victim->pcdata->flags, PCFLAG_DND ) )
    {
      send_to_char( "That player has their 'do not disturb' flag on.\n\r", ch );
      return;
    }

  if( xIS_SET( victim->deaf, CHANNEL_TELLS ) && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
  {
    act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if( is_swear( argument ) && ( !xIS_SET( victim->act, PLR_TELL_SWEAR ) || !xIS_SET( ch->act, PLR_TELL_SWEAR ) ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_SILENCE ) )
    send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );

  if( !IS_NPC( victim ) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) )
  {
    act( AT_PLAIN, "A magic force prevents your message from being heard.", ch, 0, victim, TO_CHAR );
    return;
  }

  if( victim->desc  /* make sure desc exists first  -Thoric */
      && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_GOD )
  {
    act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
    return;
  }

  /*
   * Check to see if target of tell is ignoring the sender 
   */
  if( is_ignoring( victim, ch ) )
  {
    /*
     * If the sender is an imm then they cannot be ignored 
     */
    if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
    {
      set_char_color( AT_IGNORE, ch );
      ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
      return;
    }
    /*
     * else
     * *  {
     * *    set_char_color(AT_IGNORE, victim);
     * *    ch_printf(victim, "You attempt to ignore %s, but "
     * *      "are unable to do so.\n\r", ch->name);
     * *  }
     */
  }

  ch->retell = victim;

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  if( !IS_NPC( victim ) && IS_IMMORTAL( victim ) && victim->pcdata->tell_history &&
      isalpha( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) )
  {
    sprintf( buf, "%s told you '%s'\n\r", capitalize( IS_NPC( ch ) ? ch->short_descr : ch->name ), argument );

    /*
     * get lasttell index... assumes names begin with characters 
     */
    victim->pcdata->lt_index = tolower( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) - 'a';

    /*
     * get rid of old messages 
     */
    if( victim->pcdata->tell_history[victim->pcdata->lt_index] )
      STRFREE( victim->pcdata->tell_history[victim->pcdata->lt_index] );

    /*
     * store the new message 
     */
    victim->pcdata->tell_history[victim->pcdata->lt_index] = STRALLOC( buf );
  }

  if( switched_victim )
    victim = switched_victim;


  /*
   * Bug fix by guppy@wavecomputers.net 
   */
  MOBtrigger = FALSE;

  act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
  if( !IS_AWAKE( ch ) )
  {
    char stbuf[MAX_STRING_LENGTH];
    sprintf( stbuf, "You tell %s '%s'\n\r",
             victim->morph == NULL ?
             ( ch ? PERS( victim, ch ) : ( IS_NPC( victim ) ? victim->short_descr : victim->name ) ) : "Morph", argument );
    set_char_color( AT_TELL, ch );
    send_to_char_color( stbuf, ch );
  }

  position = victim->position;
  victim->position = POS_STANDING;
  if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
  {
    int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                           knows_language( ch, ch->speaking, victim ) );

    if( speakswell < 85 )
      act( AT_TELL, "$n tells you '$t'", ch, translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
    else
      act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
  }
  else
    act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );

  MOBtrigger = TRUE;

  victim->position = position;
  victim->reply = ch;
  if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
  {
    sprintf( buf, "%s: %s (tell to) %s.",
             IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
    append_to_file( LOG_FILE, buf );
  }

  if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_ROBOT ) )
  {
    chatperform( victim, ch, argument, 2 );
  }

  sysdata.outBytesFlag = LOGBOUTNORM;
  mprog_speech_trigger( argument, ch );
  return;
}



void do_reply( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int position;
#ifndef SCRAMBLE

  int speaking = -1, lang;

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif


  xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
  if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
  {
    send_to_char( "You can't do that here.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_SILENCE ) )
  {
    send_to_char( "Your message didn't get through.\n\r", ch );
    return;
  }

  if( ( victim = ch->reply ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && ( victim->switched ) && can_see( ch, victim ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
  {
    send_to_char( "That player is switched.\n\r", ch );
    return;
  }
  else if( !IS_NPC( victim ) && ( !victim->desc ) )
  {
    send_to_char( "That player is link-dead.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) )
    if( xIS_SET( victim->act, PLR_AFK ) )
    {
      send_to_char( "That player is afk.\n\r", ch );
      return;
    }

  if( xIS_SET( victim->deaf, CHANNEL_TELLS ) && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
  {
    act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if( !IS_NPC( victim ) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) )
  {
    act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
    return;
  }

  if( victim->desc  /* make sure desc exists first  -Thoric */
      && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_GOD )
  {
    act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
    return;
  }

  if( is_swear( argument ) && ( !xIS_SET( victim->act, PLR_TELL_SWEAR ) || !xIS_SET( ch->act, PLR_TELL_SWEAR ) ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  /*
   * Check to see if the receiver is ignoring the sender 
   */
  if( is_ignoring( victim, ch ) )
  {
    /*
     * If the sender is an imm they cannot be ignored 
     */
    if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
    {
      set_char_color( AT_IGNORE, ch );
      ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
      return;
    }
    /*
     * else
     * *      {
     * *        set_char_color(AT_IGNORE, victim);
     * *        ch_printf(victim, "You attempt to ignore %s, but "
     * *          "are unable to do so.\n\r", ch->name);
     * *      }
     */
  }

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
  if( !IS_AWAKE( ch ) )
  {
    char stbuf[MAX_STRING_LENGTH];
    sprintf( stbuf, "You tell %s '%s'\n\r",
             victim->morph == NULL ?
             ( ch ? PERS( victim, ch ) : ( IS_NPC( victim ) ? victim->short_descr : victim->name ) ) : "Morph", argument );
    set_char_color( AT_TELL, ch );
    send_to_char_color( stbuf, ch );
  }
  position = victim->position;
  victim->position = POS_STANDING;
#ifndef SCRAMBLE

  if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
  {
    int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                           knows_language( ch, ch->speaking, victim ) );

    if( speakswell < 85 )
      act( AT_TELL, "$n tells you '$t'", ch, translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
    else
      act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
  }
  else
    act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
#else

  if( knows_language( victim, ch->speaking, ch ) || ( IS_NPC( ch ) && !ch->speaking ) )
    act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
  else
    act( AT_TELL, "$n tells you '$t'", ch, scramble( argument, ch->speaking ), victim, TO_VICT );
#endif

  victim->position = position;
  victim->reply = ch;
  ch->retell = victim;
  if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
  {
    sprintf( buf, "%s: %s (reply to) %s.",
             IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
    append_to_file( LOG_FILE, buf );
  }

  if( !IS_NPC( victim ) && IS_IMMORTAL( victim ) && victim->pcdata->tell_history &&
      isalpha( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) )
  {
    sprintf( buf, "%s told you '%s'\n\r", capitalize( IS_NPC( ch ) ? ch->short_descr : ch->name ), argument );

    /*
     * get lasttell index... assumes names begin with characters 
     */
    victim->pcdata->lt_index = tolower( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) - 'a';

    /*
     * get rid of old messages 
     */
    if( victim->pcdata->tell_history[victim->pcdata->lt_index] )
      STRFREE( victim->pcdata->tell_history[victim->pcdata->lt_index] );

    /*
     * store the new message 
     */
    victim->pcdata->tell_history[victim->pcdata->lt_index] = STRALLOC( buf );
  }

  if( IS_NPC( victim ) && xIS_SET( victim->act, ACT_ROBOT ) )
  {
    chatperform( victim, ch, argument, 2 );
  }

  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_retell( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int position;
  CHAR_DATA *switched_victim = NULL;
#ifndef SCRAMBLE

  int speaking = -1, lang;

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif
  xREMOVE_BIT( ch->deaf, CHANNEL_TELLS );
  if( xIS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
  {
    send_to_char( "You can't do that here.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ( xIS_SET( ch->act, PLR_SILENCE ) || xIS_SET( ch->act, PLR_NO_TELL ) ) )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "What message do you wish to send?\n\r" );
    return;
  }

  victim = ch->retell;

  if( !victim )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && ( victim->switched ) &&
      ( get_trust( ch ) > LEVEL_AVATAR ) && !IS_AFFECTED( victim->switched, AFF_POSSESS ) )
  {
    send_to_char( "That player is switched.\n\r", ch );
    return;
  }
  else if( !IS_NPC( victim ) && ( victim->switched ) && IS_AFFECTED( victim->switched, AFF_POSSESS ) )
  {
    switched_victim = victim->switched;
  }
  else if( !IS_NPC( victim ) && ( !victim->desc ) )
  {
    send_to_char( "That player is link-dead.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) )
    if( xIS_SET( victim->act, PLR_AFK ) )
    {
      send_to_char( "That player is afk.\n\r", ch );
      return;
    }

  if( xIS_SET( victim->deaf, CHANNEL_TELLS ) && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
  {
    act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if( is_swear( argument ) && ( !xIS_SET( victim->act, PLR_TELL_SWEAR ) || !xIS_SET( ch->act, PLR_TELL_SWEAR ) ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  if( !IS_NPC( victim ) && xIS_SET( victim->act, PLR_SILENCE ) )
    send_to_char( "That player is silenced. They will receive your message, but can not respond.\n\r", ch );

  if( ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) ) ||
      ( !IS_NPC( victim ) && xIS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
  {
    act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
    return;
  }

  if( victim->desc && victim->desc->connected == CON_EDITING && get_trust( ch ) < LEVEL_GOD )
  {
    act( AT_PLAIN, "$E is currently in a writing buffer. Please " "try again in a few minutes.", ch, 0, victim, TO_CHAR );
    return;
  }

  /*
   * check to see if the target is ignoring the sender 
   */
  if( is_ignoring( victim, ch ) )
  {
    /*
     * if the sender is an imm then they cannot be ignored 
     */
    if( !IS_IMMORTAL( ch ) || get_trust( victim ) > get_trust( ch ) )
    {
      set_char_color( AT_IGNORE, ch );
      ch_printf( ch, "%s is ignoring you.\n\r", victim->name );
      return;
    }
    else
    {
      set_char_color( AT_IGNORE, victim );
      ch_printf( victim, "You attempt to ignore %s, but " "are unable to do so.\n\r", ch->name );
    }
  }

  /*
   * store tell history for victim 
   */
  if( !IS_NPC( victim ) && IS_IMMORTAL( victim ) && victim->pcdata->tell_history &&
      isalpha( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) )
  {
    sprintf( buf, "%s told you '%s'\n\r", capitalize( IS_NPC( ch ) ? ch->short_descr : ch->name ), argument );

    /*
     * get lasttel index... assumes names begin with chars 
     */
    victim->pcdata->lt_index = tolower( IS_NPC( ch ) ? ch->short_descr[0] : ch->name[0] ) - 'a';

    /*
     * get rid of old messages 
     */
    if( victim->pcdata->tell_history[victim->pcdata->lt_index] )
      STRFREE( victim->pcdata->tell_history[victim->pcdata->lt_index] );

    /*
     * store the new messagec 
     */
    victim->pcdata->tell_history[victim->pcdata->lt_index] = STRALLOC( buf );
  }

  if( switched_victim )
    victim = switched_victim;

  /*
   * Bug fix by guppy@wavecomputers.net 
   */
  MOBtrigger = FALSE;

  act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
  if( !IS_AWAKE( ch ) )
  {
    char stbuf[MAX_STRING_LENGTH];
    sprintf( stbuf, "You tell %s '%s'\n\r",
             victim->morph == NULL ?
             ( ch ? PERS( victim, ch ) : ( IS_NPC( victim ) ? victim->short_descr : victim->name ) ) : "Morph", argument );
    set_char_color( AT_TELL, ch );
    send_to_char_color( stbuf, ch );
  }
  position = victim->position;
  victim->position = POS_STANDING;
#ifndef SCRAMBLE

  if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
  {
    int speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                           knows_language( ch, ch->speaking, victim ) );

    if( speakswell < 85 )
      act( AT_TELL, "$n tells you '$t'", ch, translate( speakswell, argument, lang_names[speaking] ), victim, TO_VICT );
    else
      act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
  }
  else
    act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
#else

  if( knows_language( victim, ch->speaking, ch ) || ( IS_NPC( ch ) && !ch->speaking ) )
  {
    act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
  }
  else
  {
    act( AT_TELL, "$n tells you '$t'", ch, scramble( argument, ch->speaking ), victim, TO_VICT );
  }
#endif

  MOBtrigger = TRUE;

  victim->position = position;
  victim->reply = ch;
  if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
  {
    sprintf( buf, "%s: %s (retell to) %s.",
             IS_NPC( ch ) ? ch->short_descr : ch->name, argument, IS_NPC( victim ) ? victim->short_descr : victim->name );
    append_to_file( LOG_FILE, buf );
  }

  mprog_speech_trigger( argument, ch );
  return;
}

void do_repeat( CHAR_DATA * ch, char *argument )
{
  int index;

  if( IS_NPC( ch ) || !IS_IMMORTAL( ch ) || !ch->pcdata->tell_history )
  {
    ch_printf( ch, "Huh?\n\r" );
    return;
  }

  if( argument[0] == '\0' )
  {
    index = ch->pcdata->lt_index;
  }
  else if( isalpha( argument[0] ) && argument[1] == '\0' )
  {
    index = tolower( argument[0] ) - 'a';
  }
  else
  {
    ch_printf( ch, "You may only index your tell history using " "a single letter.\n\r" );
    return;
  }

  if( ch->pcdata->tell_history[index] )
  {
    set_char_color( AT_TELL, ch );
    ch_printf( ch, ch->pcdata->tell_history[index] );
  }
  else
  {
    ch_printf( ch, "No one like that has sent you a tell.\n\r" );
  }

  return;
}


void do_emote( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char *plast;
  CHAR_DATA *vch;
  EXT_BV actflags;
#ifndef SCRAMBLE

  int speaking = -1, lang;

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif

  if( !IS_NPC( ch ) && xIS_SET( ch->act, PLR_NO_EMOTE ) )
  {
    send_to_char( "You can't show your emotions.\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    send_to_char( "Emote what?\n\r", ch );
    return;
  }

  actflags = ch->act;
  if( IS_NPC( ch ) )
    xREMOVE_BIT( ch->act, ACT_SECRETIVE );
  for( plast = argument; *plast != '\0'; plast++ )
    ;

  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    ch_printf( ch, "You can't while Oozaru.\n\r" );
    return;
  }
  strcpy( buf, argument );
  if( isalpha( plast[-1] ) )
    strcat( buf, "." );
  for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
  {
    char *sbuf = buf;

    /*
     * Check to see if character is ignoring emoter 
     */
    if( is_ignoring( vch, ch ) )
    {
      /*
       * continue unless emoter is an immortal 
       */
      if( !IS_IMMORTAL( ch ) || get_trust( vch ) > get_trust( ch ) )
        continue;
      /*
       * else
       * *      {
       * *        set_char_color(AT_IGNORE, vch);
       * *        ch_printf(vch,"You attempt to ignore %s, but"
       * *          " are unable to do so.\n\r", ch->name);
       * *      }
       */
    }

    /*
     * if (IS_NPC(ch) && xIS_SET(ch->act, ACT_ROBOT))
     * {
     * chatperformtoroom(argument,ch);
     * return;
     * }
     */

#ifndef SCRAMBLE
    if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
    {
      int speakswell = UMIN( knows_language( vch, ch->speaking, ch ),
                             knows_language( ch, ch->speaking, vch ) );

      if( speakswell < 85 )
        sbuf = translate( speakswell, argument, lang_names[speaking] );
    }
#else
    if( !knows_language( vch, ch->speaking, ch ) && ( !IS_NPC( ch ) && ch->speaking != 0 ) )
      sbuf = scramble( buf, ch->speaking );
#endif

    MOBtrigger = FALSE;
    sysdata.outBytesFlag = LOGBOUTCHANNEL;
    act( AT_ACTION, "$n $t", ch, sbuf, vch, ( vch == ch ? TO_CHAR : TO_VICT ) );
    sysdata.outBytesFlag = LOGBOUTNORM;
  }
  /*
   * MOBtrigger = FALSE;
   * act( AT_ACTION, "$n $T", ch, NULL, buf, TO_ROOM );
   * MOBtrigger = FALSE;
   * act( AT_ACTION, "$n $T", ch, NULL, buf, TO_CHAR );
   */
  ch->act = actflags;
  if( xIS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
  {
    sprintf( buf, "%s %s (emote)", IS_NPC( ch ) ? ch->short_descr : ch->name, argument );
    append_to_file( LOG_FILE, buf );
  }
  return;
}


void do_bug( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  struct tm *t = localtime( &current_time );

  set_char_color( AT_PLAIN, ch );
  if( argument[0] == '\0' )
  {
    send_to_char( "\n\rUsage:  'bug <message>'  (your location is automatically recorded)\n\r", ch );
    return;
  }
  sprintf( buf, "(%-2.2d/%-2.2d):  %s", t->tm_mon + 1, t->tm_mday, argument );
  append_file( ch, PBUG_FILE, buf );
  send_to_char( "Thanks, your bug notice has been recorded.\n\r", ch );
  return;
}

void do_ide( CHAR_DATA * ch, char *argument )
{
  set_char_color( AT_PLAIN, ch );
  send_to_char( "\n\rIf you want to send an idea, type 'idea <message>'.\n\r", ch );
  send_to_char( "If you want to identify an object, use the identify spell.\n\r", ch );
  return;
}

void do_idea( CHAR_DATA * ch, char *argument )
{
  set_char_color( AT_PLAIN, ch );
  if( argument[0] == '\0' )
  {
    send_to_char( "\n\rUsage:  'idea <message>'\n\r", ch );
    return;
  }
  append_file( ch, IDEA_FILE, argument );
  send_to_char( "Thanks, your idea has been recorded.\n\r", ch );
  return;
}

/* Check the add helpfile file -- Xerves 8/24/99 */
void do_ahelp( CHAR_DATA * ch, char *argument )
{
  set_char_color( AT_PLAIN, ch );
  if( argument[0] == '\0' )
  {
    send_to_char_color( "\n\rUsage:  'ahelp list' or 'ahelp clear now'\n\r", ch );
    return;
  }
  if( !str_cmp( argument, "clear now" ) )
  {
    FILE *fp = fopen( HELP_FILE, "w" );
    if( fp )
      fclose( fp );
    send_to_char_color( "Add Help file cleared.\n\r", ch );
    return;
  }
  if( !str_cmp( argument, "list" ) )
  {
    send_to_char_color( "\n\r VNUM \n\r.......\n\r", ch );
    show_file( ch, HELP_FILE );
  }
  else
  {
    send_to_char_color( "\n\rUsage:  'ahelp list' or 'ahelp clear now'\n\r", ch );
    return;
  }
  return;
}

void do_typo( CHAR_DATA * ch, char *argument )
{
  set_char_color( AT_PLAIN, ch );
  if( argument[0] == '\0' )
  {
    send_to_char( "\n\rUsage:  'typo <message>'  (your location is automatically recorded)\n\r", ch );
    if( get_trust( ch ) >= LEVEL_ASCENDANT )
      send_to_char( "Usage:  'typo list' or 'typo clear now'\n\r", ch );
    return;
  }
  if( !str_cmp( argument, "clear now" ) && get_trust( ch ) >= LEVEL_ASCENDANT )
  {
    FILE *fp = fopen( TYPO_FILE, "w" );
    if( fp )
      fclose( fp );
    send_to_char( "Typo file cleared.\n\r", ch );
    return;
  }
  if( !str_cmp( argument, "list" ) && get_trust( ch ) >= LEVEL_ASCENDANT )
  {
    send_to_char( "\n\r VNUM \n\r.......\n\r", ch );
    show_file( ch, TYPO_FILE );
  }
  else
  {
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Thanks, your typo notice has been recorded.\n\r", ch );
  }
  return;
}

void do_rent( CHAR_DATA * ch, char *argument )
{
  set_char_color( AT_WHITE, ch );
  send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
  return;
}



void do_qui( CHAR_DATA * ch, char *argument )
{
  set_char_color( AT_RED, ch );
  send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
  return;
}

void do_quit( CHAR_DATA * ch, char *argument )
{
  AUTH_LIST *old_auth;
  OBJ_DATA *obj;
  OBJ_DATA *o;
  int x, y;
  int level;
  char buf[MAX_STRING_LENGTH];

  if( IS_NPC( ch ) )
    return;

  if( ch->position == POS_FIGHTING
      || ch->position == POS_EVASIVE
      || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
  {
    set_char_color( AT_RED, ch );
    send_to_char( "No way! You are fighting.\n\r", ch );
    return;
  }

  if( ch->position < POS_STUNNED )
  {
    set_char_color( AT_BLOOD, ch );
    send_to_char( "You're not DEAD yet.\n\r", ch );
    return;
  }
  if( ch->position == POS_STUNNED )
  {
    set_char_color( AT_BLOOD, ch );
    send_to_char( "You're stunned, you can quit yet.\n\r", ch );
    return;
  }


  if( get_timer( ch, TIMER_RECENTFIGHT ) > 0 && !IS_IMMORTAL( ch ) )
  {
    set_char_color( AT_RED, ch );
    send_to_char( "Your adrenaline is pumping too hard to quit now!\n\r", ch );
    return;
  }

  if( auction->item != NULL && ( ( ch == auction->buyer ) || ( ch == auction->seller ) ) )
  {
    send_to_char( "Wait until you have bought/sold the item on auction.\n\r", ch );
    return;

  }

  while( ( o = carrying_noquit( ch ) ) != NULL )
  {
    obj_from_char( o );
    obj_to_room( o, ch->in_room );
    ch_printf( ch, "&wYou drop %s&w.\n\r", o->short_descr );
  }

  if( IS_PKILL( ch ) && ch->wimpy > ( int )ch->max_hit / 2.25 )
  {
    send_to_char( "Your wimpy has been adjusted to the maximum level for deadlies.\n\r", ch );
    do_wimpy( ch, "max" );
  }
  /*
   * Purge all doubles no mater what, we don't want leftovers for dinner :) -Goku 
   */
  if( is_splitformed( ch ) )
  {
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    for( och = first_char; och; och = och_next )
    {
      och_next = och->next;

      if( !IS_NPC( och ) )
        continue;

      if( ( xIS_SET( och->affected_by, AFF_SPLIT_FORM )
            || xIS_SET( och->affected_by, AFF_TRI_FORM )
            || xIS_SET( och->affected_by, AFF_MULTI_FORM ) || xIS_SET( och->affected_by, AFF_BIOJR ) ) && och->master == ch )
      {
        extract_char( och, TRUE );
      }
    }
    xREMOVE_BIT( ( ch )->affected_by, AFF_MULTI_FORM );
    xREMOVE_BIT( ( ch )->affected_by, AFF_TRI_FORM );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SPLIT_FORM );
    xREMOVE_BIT( ( ch )->affected_by, AFF_BIOJR );
  }

  old_auth = get_auth_name( ch->name );
  if( old_auth != NULL && ( old_auth->state == AUTH_ONLINE || old_auth->state == AUTH_LINK_DEAD ) )
    old_auth->state = AUTH_OFFLINE; /* Logging off */

  /*
   * Get 'em dismounted until we finish mount saving -- Blodkai, 4/97 
   */
  if( ch->position == POS_MOUNTED )
    do_dismount( ch, "" );

  set_char_color( AT_WHITE, ch );
  send_to_char
    ( "Your surroundings begin to fade as a mystical swirling vortex of colors\n\renvelops your body... When you come to, things are not as they were.\n\r\n\r",
      ch );
  act( AT_SAY, "A strange voice says, 'We await your return, $n...'", ch, NULL, NULL, TO_CHAR );
  act( AT_BYE, "$n has left the game.", ch, NULL, NULL, TO_CANSEE );

  adjust_hiscore( "played", ch, ( get_age( ch ) - 4 ) );
  adjust_hiscore( "zeni", ch, ch->gold );

  sprintf( buf, "%s has left the game", ch->name );
  if( !IS_IMMORTAL( ch ) )
    do_info( ch, buf );
  else
    do_ainfo( ch, buf );

  if( xIS_SET( ch->affected_by, AFF_TAG ) )
    xREMOVE_BIT( ch->affected_by, AFF_TAG );

  set_char_color( AT_GREY, ch );

  sprintf( log_buf, "%s has quit (Room %d).", ch->name, ( ch->in_room ? ch->in_room->vnum : -1 ) );
  quitting_char = ch;
  save_char_obj( ch );

  if( sysdata.save_pets && ch->pcdata->pet )
  {
    act( AT_BYE, "$N follows $S master into the Void.", ch, NULL, ch->pcdata->pet, TO_ROOM );
    extract_char( ch->pcdata->pet, TRUE );
  }

  if( ch->pcdata->in_progress )
    free_global_note( ch->pcdata->in_progress );


  /*
   * Synch clandata up only when clan member quits now. --Shaddai
   */
  if( ch->pcdata->clan )
    save_clan( ch->pcdata->clan );

  saving_char = NULL;

  level = get_trust( ch );
  /*
   * After extract_char the ch is no longer valid!
   */
  for( obj = ch->first_carrying; obj; obj = obj->next_content )
  {
    if( IS_OBJ_STAT( obj, ITEM_RARE ) || IS_OBJ_STAT( obj, ITEM_UNIQUE ) )
      obj->pIndexData->count += obj->count;
  }
  extract_char( ch, TRUE );
  for( x = 0; x < MAX_WEAR; x++ )
    for( y = 0; y < MAX_LAYERS; y++ )
      save_equipment[x][y] = NULL;

  /*
   * don't show who's logging off to leaving player 
   */
  /*
   * to_channel( log_buf, CHANNEL_MONITOR, "Monitor", level );
   */
  log_string_plus( log_buf, LOG_COMM, level );
  return;
}


void send_rip_screen( CHAR_DATA * ch )
{
  FILE *rpfile;
  int num = 0;
  char BUFF[MAX_STRING_LENGTH * 2];

  if( ( rpfile = fopen( RIPSCREEN_FILE, "r" ) ) != NULL )
  {
    while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
      num++;
    fclose( rpfile );
    BUFF[num] = 0;
    write_to_buffer( ch->desc, BUFF, num );
  }
}

void send_rip_title( CHAR_DATA * ch )
{
  FILE *rpfile;
  int num = 0;
  char BUFF[MAX_STRING_LENGTH * 2];

  if( ( rpfile = fopen( RIPTITLE_FILE, "r" ) ) != NULL )
  {
    while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
      num++;
    fclose( rpfile );
    BUFF[num] = 0;
    write_to_buffer( ch->desc, BUFF, num );
  }
}

void send_ansi_title( CHAR_DATA * ch )
{
  FILE *rpfile;
  int num = 0;
  char BUFF[MAX_STRING_LENGTH * 2];

  if( ( rpfile = fopen( ANSITITLE_FILE, "r" ) ) != NULL )
  {
    while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
      num++;
    fclose( rpfile );
    BUFF[num] = 0;
    write_to_buffer( ch->desc, BUFF, num );
  }
}

void send_ascii_title( CHAR_DATA * ch )
{
  FILE *rpfile;
  int num = 0;
  char BUFF[MAX_STRING_LENGTH];

  if( ( rpfile = fopen( ASCTITLE_FILE, "r" ) ) != NULL )
  {
    while( ( BUFF[num] = fgetc( rpfile ) ) != EOF )
      num++;
    fclose( rpfile );
    BUFF[num] = 0;
    write_to_buffer( ch->desc, BUFF, num );
  }
}

void do_omenu( CHAR_DATA * ch, char *argument )
{
  OBJ_DATA *obj;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  strcpy( arg3, argument );

  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: omenu <object> [page]  \n\r", ch );
    send_to_char( "      Where:    <object> is a prototype object  \n\r", ch );
    send_to_char( "            and  <page>  is an optional letter to select menu-pages\n\r", ch );
    return;
  }

  if( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  /*
   * can redit or something 
   */

  ch->inter_type = OBJ_TYPE;
  ch->inter_substate = SUB_NORTH;
  if( ch->inter_editing != NULL )
    DISPOSE( ch->inter_editing );
  ch->inter_editing = str_dup( obj->pIndexData->name );
  sscanf( ch->inter_editing, "%s", ch->inter_editing ); /*one-arg */
  ch->inter_editing_vnum = obj->pIndexData->vnum;
  send_obj_page_to_char( ch, obj->pIndexData, arg2[0] );
}


void do_rmenu( CHAR_DATA * ch, char *argument )
{
  ROOM_INDEX_DATA *idx;
  char arg1[MAX_INPUT_LENGTH];

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );

  idx = ch->in_room;
  /*
   * can redit or something 
   */

  ch->inter_type = ROOM_TYPE;
  ch->inter_substate = SUB_NORTH;
  if( ch->inter_editing != NULL )
    DISPOSE( ch->inter_editing );
  ch->inter_editing = str_dup( idx->name );
  sscanf( ch->inter_editing, "%s", ch->inter_editing ); /*one-arg */
  ch->inter_editing_vnum = idx->vnum;
  send_room_page_to_char( ch, idx, arg1[0] );
}

void do_cmenu( CHAR_DATA * ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );

  ch->inter_type = CONTROL_TYPE;
  if( ch->inter_editing != NULL )
    DISPOSE( ch->inter_editing );
  ch->inter_editing = str_dup( "Control Panel" );
  sscanf( ch->inter_editing, "%s", ch->inter_editing ); /*one-arg */
  send_control_page_to_char( ch, arg1[0] );
}


void do_mmenu( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  strcpy( arg3, argument );

  if( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: mmenu <victim> [page]  \n\r", ch );
    send_to_char( "      Where:    <victim> is a prototype mob  \n\r", ch );
    send_to_char( "            and  <page>  is an optional letter to select menu-pages\n\r", ch );
    return;
  }


  if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) )
  {
    send_to_char( "Not on players.\n\r", ch );
    return;
  }

  if( get_trust( ch ) < victim->level )
  {
    set_char_color( AT_IMMORT, ch );
    send_to_char( "Their godly glow prevents you from getting a good look .\n\r", ch );
    return;
  }
  ch->inter_type = MOB_TYPE;
  if( ch->inter_editing != NULL )
    DISPOSE( ch->inter_editing );
  ch->inter_editing = str_dup( arg1 );
  sscanf( ch->inter_editing, "%s", ch->inter_editing ); /*one-arg */
  ch->inter_editing_vnum = victim->pIndexData->vnum;
  send_page_to_char( ch, victim->pIndexData, arg2[0] );
}


void do_rip( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Rip ON or OFF?\n\r", ch );
    return;
  }
  if( ( strcmp( arg, "on" ) == 0 ) || ( strcmp( arg, "ON" ) == 0 ) )
  {
    send_rip_screen( ch );
    xSET_BIT( ch->act, PLR_RIP );
    xSET_BIT( ch->act, PLR_ANSI );
    return;
  }

  if( ( strcmp( arg, "off" ) == 0 ) || ( strcmp( arg, "OFF" ) == 0 ) )
  {
    xREMOVE_BIT( ch->act, PLR_RIP );
    send_to_char( "!|*\n\rRIP now off...\n\r", ch );
    return;
  }
}

void do_ansi( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "ANSI ON or OFF?\n\r", ch );
    return;
  }
  if( ( strcmp( arg, "on" ) == 0 ) || ( strcmp( arg, "ON" ) == 0 ) )
  {
    xSET_BIT( ch->act, PLR_ANSI );
    set_char_color( AT_WHITE + AT_BLINK, ch );
    send_to_char( "ANSI ON!!!\n\r", ch );
    return;
  }

  if( ( strcmp( arg, "off" ) == 0 ) || ( strcmp( arg, "OFF" ) == 0 ) )
  {
    xREMOVE_BIT( ch->act, PLR_ANSI );
    send_to_char( "Okay... ANSI support is now off\n\r", ch );
    return;
  }
}

void do_save( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) )
    return;

  if( NOT_AUTHED( ch ) && ch->exp < 4000 )
  {
    send_to_char_color
      ( "&BYou must have a power level greater than 4,000 to be allowed\n\rto save with out having your name authorized.\n\r",
        ch );
    return;
  }
  WAIT_STATE( ch, 2 );  /* For big muds with save-happy players, like RoD */
  update_aris( ch );  /* update char affects and RIS */
  save_char_obj( ch );
  saving_char = NULL;
  send_to_char( "&wSaved...\n\r", ch );
  adjust_hiscore( "pkill", ch, ch->pcdata->pkills );  /* cronel hiscore */
  adjust_hiscore( "sparwins", ch, ch->pcdata->spar_wins );
  adjust_hiscore( "sparloss", ch, ch->pcdata->spar_loss );
  adjust_hiscore( "mkills", ch, ch->pcdata->mkills );
  adjust_hiscore( "deaths", ch, ( ch->pcdata->pdeaths + ch->pcdata->mdeaths ) );
  update_plHiscore( ch );
  adjust_hiscore( "played", ch, ( get_age( ch ) - 4 ) );
  adjust_hiscore( "zeni", ch, ch->gold );
  adjust_hiscore( "bounty", ch, ch->pcdata->bkills );
  update_member( ch );
  rank_update(  );
  return;
}


/*
 * Something from original DikuMUD that Merc yanked out.
 * Used to prevent following loops, which can cause problems if people
 * follow in a loop through an exit leading back into the same room
 * (Which exists in many maze areas)			-Thoric
 */
bool circle_follow( CHAR_DATA * ch, CHAR_DATA * victim )
{
  CHAR_DATA *tmp;

  for( tmp = victim; tmp; tmp = tmp->master )
    if( tmp == ch )
      return TRUE;
  return FALSE;
}


void do_dismiss( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Dismiss whom?\n\r", ch );
    return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( ( IS_AFFECTED( victim, AFF_CHARM ) ) && ( IS_NPC( victim ) ) && ( victim->master == ch ) )
  {
    stop_follower( victim );
    stop_hating( victim );
    stop_hunting( victim );
    stop_fearing( victim );
    act( AT_ACTION, "$n dismisses $N.", ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "You dismiss $N.", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    send_to_char( "You cannot dismiss them.\n\r", ch );
  }

  return;
}

void do_follow( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if( IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SPLIT_FORM ) )
    return;

  if( arg[0] == '\0' )
  {
    send_to_char( "Follow whom?\n\r", ch );
    return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( ( ( !IS_NPC( ch ) && xIS_SET( victim->act, PLR_NOFOLLOW ) && !IS_IMMORTAL( ch ) )
        || ( IS_NPC( victim ) && xIS_SET( victim->act, ACT_NOFOLLOW ) ) ) && ch != victim )
  {
    send_to_char( "They don't want any followers.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_CHARM ) && ch->master )
  {
    act( AT_PLAIN, "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
    return;
  }

  if( is_splitformed( ch ) && ch->master )
  {
    act( AT_PLAIN, "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
    return;
  }

  if( victim == ch )
  {
    if( !ch->master )
    {
      send_to_char( "You already follow yourself.\n\r", ch );
      return;
    }
    stop_follower( ch );
    return;
  }

/*
  if( ( ch->level - victim->level < -10 || ch->level - victim->level > 10 )
      && !IS_HERO( ch ) && !( ch->level < 15 && !IS_NPC( victim )
                              && victim->pcdata->council && !str_cmp( victim->pcdata->council->name, "Newbie Council" ) ) )
  {
    send_to_char( "You are not of the right caliber to follow.\n\r", ch );
    return;
  }
*/

  if( circle_follow( ch, victim ) )
  {
    send_to_char( "Following in loops is not allowed... sorry.\n\r", ch );
    return;
  }

  if( ch->master )
    stop_follower( ch );

  add_follower( ch, victim );
  return;
}



void add_follower( CHAR_DATA * ch, CHAR_DATA * master )
{
  if( ch->master )
  {
    bug( "Add_follower: non-null master.", 0 );
    return;
  }

  ch->master = master;
  ch->leader = NULL;

  /*
   * Support for saving pets --Shaddai 
   */
  if( IS_NPC( ch ) && xIS_SET( ch->act, ACT_PET ) && !IS_NPC( master ) )
    master->pcdata->pet = ch;

  if( can_see( master, ch ) )
    act( AT_ACTION, "$n now follows you.", ch, NULL, master, TO_VICT );

  act( AT_ACTION, "You now follow $N.", ch, NULL, master, TO_CHAR );

  return;
}



void stop_follower( CHAR_DATA * ch )
{
  if( !ch->master )
  {
    bug( "Stop_follower: null master.", 0 );
    return;
  }

  if( IS_NPC( ch ) && !IS_NPC( ch->master ) && ch->master->pcdata->pet == ch )
    ch->master->pcdata->pet = NULL;

  if( IS_AFFECTED( ch, AFF_CHARM ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_CHARM );
    affect_strip( ch, gsn_charm_person );
    if( !IS_NPC( ch->master ) )
      ch->master->pcdata->charmies--;
  }

  if( can_see( ch->master, ch ) )
    if( !( !IS_NPC( ch->master ) && IS_IMMORTAL( ch ) && !IS_IMMORTAL( ch->master ) ) )
      act( AT_ACTION, "$n stops following you.", ch, NULL, ch->master, TO_VICT );
  act( AT_ACTION, "You stop following $N.", ch, NULL, ch->master, TO_CHAR );

  ch->master = NULL;
  ch->leader = NULL;
  return;
}



void die_follower( CHAR_DATA * ch )
{
  CHAR_DATA *fch;

  if( ch->master )
    stop_follower( ch );

  ch->leader = NULL;

  for( fch = first_char; fch; fch = fch->next )
  {
    if( fch->master == ch )
      stop_follower( fch );
    if( fch->leader == ch )
      fch->leader = fch;
  }
  return;
}



void do_order( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char argbuf[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *och;
  CHAR_DATA *och_next;
  bool found;
  bool fAll;

  strcpy( argbuf, argument );
  argument = one_argument( argument, arg );
  one_argument( argument, arg2 );

  if( arg[0] == '\0' || argument[0] == '\0' )
  {
    send_to_char( "Order whom to do what?\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
    return;
  }

  /*
   * if( xIS_SET( ch->act, PLR_SPAR ) )
   * {
   * send_to_char( "For the time being you can't do this while sparring.\n\r", ch );
   * return;
   * }
   */
  if( !str_cmp( arg2, "jibaku" ) && !IS_SAIBAMAN(ch) )
  {
    send_to_char( "&ROnly Saibaman can do this.. you dork. - Kal.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "mpmload" ) )
  {
    send_to_char( "NO CHEATING IN MEH HOUSE!! - Kalthizar.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "mpoload" ) )
  {
    send_to_char( "NO CHEATING IN MEH HOUSE!! - Kalthizar.\n\r", ch );
    return;
  }
  if( !str_cmp( arg2, "spar" ) )
  {
    send_to_char( "You cannot order your follower to engage in a spar.\n\r", ch );
    return;
  }

  if( !str_cmp( arg2, "kill" ) )
  {
    ch_printf( ch, "They will only attack something if you do.\n\r" );
    return;
  }

  if( !str_cmp( arg, "all" ) )
  {
    fAll = TRUE;
    victim = NULL;
  }
  else
  {
    fAll = FALSE;
    if( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }

    if( victim == ch )
    {
      send_to_char( "Aye aye, right away!\n\r", ch );
      return;
    }

    if( ( !is_splitformed( ch ) || victim->master != ch ) )
    {
      send_to_char( "Do it yourself!\n\r", ch );
      return;
    }

    if( !IS_NPC( victim ) )
    {
      send_to_char( "No chance in HFIL\n\r", ch );
      return;
    }
  }

  found = FALSE;
  for( och = ch->in_room->first_person; och; och = och_next )
  {
    och_next = och->next_in_room;

    if( !IS_NPC( och ) )
      continue;

    if( ( IS_AFFECTED( och, AFF_CHARM ) || is_splitformed( ch ) ) && och->master == ch && ( fAll || och == victim ) )
    {
      found = TRUE;
      act( AT_ACTION, "$n orders you to '$t'.", ch, argument, och, TO_VICT );
      interpret( och, argument );
    }
  }

  if( found )
  {
    /*
     * sprintf( log_buf, "%s: order %s.", ch->name, argbuf );
     * log_string_plus( log_buf, LOG_NORMAL, ch->level );
     */
    send_to_char( "Ok.\n\r", ch );
    //WAIT_STATE( ch, 12 );
  }
  else
    send_to_char( "You have no followers here.\n\r", ch );
  return;
}

/*
char *itoa(int foo)
{
  static char bar[256];

  sprintf(bar,"%d",foo);
  return(bar);

}
*/

/* Overhauled 2/97 -- Blodkai */
void do_group( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    CHAR_DATA *gch;
    CHAR_DATA *leader;

    leader = ch->leader ? ch->leader : ch;
    set_char_color( AT_DGREEN, ch );
    ch_printf( ch, "\n\rFollowing %-12.12s     [hitpnts]   [ magic ] [mst] [mvs] [race]%s\n\r",
               PERS( leader, ch ), ch->level < 50 ? " [to lvl]" : "" );
    for( gch = first_char; gch; gch = gch->next )
    {
      if( is_same_group( gch, ch ) )
      {
        set_char_color( AT_DGREEN, ch );
        if( IS_AFFECTED( gch, AFF_POSSESS ) ) /*reveal no information */
          ch_printf( ch,
                     "[%2d %s] %-16s %4s/%4s hp %4s/%4s %s %4s/%4s mv %5s xp\n\r",
                     gch->level,
                     IS_NPC( gch ) ? "Mob" : class_table[gch->class]->who_name,
                     capitalize( PERS( gch, ch ) ), "????", "????", "????", "????", "????", "????", "?????" );
        else if( gch->alignment > 750 )
          sprintf( buf, " A" );
        else if( gch->alignment > 350 )
          sprintf( buf, "-A" );
        else if( gch->alignment > 150 )
          sprintf( buf, "+N" );
        else if( gch->alignment > -150 )
          sprintf( buf, " N" );
        else if( gch->alignment > -350 )
          sprintf( buf, "-N" );
        else if( gch->alignment > -750 )
          sprintf( buf, "+S" );
        else
          sprintf( buf, " S" );
        set_char_color( AT_DGREEN, ch );
        send_to_char( "[", ch );
        set_char_color( AT_GREEN, ch );
        ch_printf( ch, "%-2d %2.2s %3.3s", get_rank_number(gch), buf, IS_NPC( gch ) ? "Mob" : class_table[gch->class]->who_name );
        set_char_color( AT_DGREEN, ch );
        send_to_char( "]  ", ch );
        set_char_color( AT_GREEN, ch );
        ch_printf( ch, "%-12.12s ", capitalize( PERS( gch, ch ) ) );
        if( gch->hit < gch->max_hit / 4 )
          set_char_color( AT_DANGER, ch );
        else if( gch->hit < gch->max_hit / 2.5 )
          set_char_color( AT_YELLOW, ch );
        else
          set_char_color( AT_GREY, ch );
        ch_printf( ch, "%5d", gch->hit );
        set_char_color( AT_GREY, ch );
        ch_printf( ch, "/%-5d ", gch->max_hit );
        set_char_color( AT_LBLUE, ch );
        ch_printf( ch, "%5d/%-5d ", gch->mana, gch->max_mana );
        if( gch->mental_state < -25 || gch->mental_state > 25 )
          set_char_color( AT_YELLOW, ch );
        else
          set_char_color( AT_GREEN, ch );
        ch_printf( ch, "%3.3s  ",
                   gch->mental_state > 75 ? "+++" :
                   gch->mental_state > 50 ? "=++" :
                   gch->mental_state > 25 ? "==+" :
                   gch->mental_state > -25 ? "===" :
                   gch->mental_state > -50 ? "-==" : gch->mental_state > -75 ? "--=" : "---" );
        set_char_color( AT_DGREEN, ch );
        ch_printf( ch, "%5d ", gch->move );
        ch_printf( ch, "%6s ",
                   gch->race == 0 ? "saiyan" :
                   gch->race == 1 ? "human" :
                   gch->race == 2 ? "halfbreed" :
                   gch->race == 3 ? "namek" :
                   gch->race == 4 ? "android" :
                   gch->race == 5 ? "icer" :
                   gch->race == 6 ? "bio-android" :
                   gch->race == 7 ? "kaio" :
                   gch->race == 8 ? "demon" : gch->race == 8 ? "majin" : gch->race == 9 ? "genie" : "" );
	/* TODO: Ommited.. cause we don't have away to calculate this yet using our special rankings yet.. -- Chili
        set_char_color( AT_GREEN, ch );
        if( get_rank_number(gch) < 50 )
          ch_printf( ch, "%8d ", exp_level( gch, gch->level + 1 ) - gch->exp );
	  */
        send_to_char( "\n\r", ch );
      }
    }
    return;
  }

  if( !strcmp( arg, "disband" ) )
  {
    CHAR_DATA *gch;
    int count = 0;

    if( ch->leader || ch->master )
    {
      send_to_char( "You cannot disband a group if you're following someone.\n\r", ch );
      return;
    }

    for( gch = first_char; gch; gch = gch->next )
    {
      if( is_same_group( ch, gch ) && ( ch != gch ) )
      {
        gch->leader = NULL;
        gch->master = NULL;
        count++;
        send_to_char( "Your group is disbanded.\n\r", gch );
      }
    }

    if( count == 0 )
      send_to_char( "You have no group members to disband.\n\r", ch );
    else
      send_to_char( "You disband your group.\n\r", ch );

    return;
  }

  if( !strcmp( arg, "all" ) )
  {
    CHAR_DATA *rch;
    int count = 0;

    for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
    {
      if( ch != rch
          && !IS_NPC( rch )
          && can_see( ch, rch )
          && rch->master == ch
          && !ch->master
          && !ch->leader
          && abs( get_rank_number(ch) - get_rank_number(ch) ) < 9 && !is_same_group( rch, ch ) && IS_PKILL( ch ) == IS_PKILL( rch ) )
      {
        rch->leader = ch;
        count++;
      }
    }

    if( count == 0 )
      send_to_char( "You have no eligible group members.\n\r", ch );
    else
    {
      act( AT_ACTION, "$n groups $s followers.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You group your followers.\n\r", ch );
    }
    return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( ch->master || ( ch->leader && ch->leader != ch ) )
  {
    send_to_char( "But you are following someone else!\n\r", ch );
    return;
  }

  if( victim->master != ch && ch != victim )
  {
    act( AT_PLAIN, "$N isn't following you.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if( victim == ch )
  {
    act( AT_PLAIN, "You can't group yourself.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if( is_same_group( victim, ch ) && ch != victim )
  {
    victim->leader = NULL;
    act( AT_ACTION, "$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "$n removes you from $s group.", ch, NULL, victim, TO_VICT );
    act( AT_ACTION, "You remove $N from your group.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if( get_rank_number(ch) - get_rank_number(victim) < -5 || get_rank_number(ch) - get_rank_number(victim) > 5 || 
      ( IS_PKILL( ch ) != IS_PKILL( victim ) ) )
  {
    act( AT_PLAIN, "$N cannot join $n's group.", ch, NULL, victim, TO_NOTVICT );
    act( AT_PLAIN, "You cannot join $n's group.", ch, NULL, victim, TO_VICT );
    act( AT_PLAIN, "$N cannot join your group.", ch, NULL, victim, TO_CHAR );
    return;
  }

  victim->leader = ch;
  act( AT_ACTION, "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
  act( AT_ACTION, "You join $n's group.", ch, NULL, victim, TO_VICT );
  act( AT_ACTION, "$N joins your group.", ch, NULL, victim, TO_CHAR );
  return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *gch;
  int members;
  int amount;
  int share;
  int extra;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Split how much?\n\r", ch );
    return;
  }

  amount = atoi( arg );

  if( amount < 0 )
  {
    send_to_char( "Your group wouldn't like that.\n\r", ch );
    return;
  }

  if( amount == 0 )
  {
    send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
    return;
  }

  if( ch->gold < amount )
  {
    send_to_char( "You don't have that much zeni.\n\r", ch );
    return;
  }

  members = 0;
  for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
  {
    if( is_same_group( gch, ch ) )
      members++;
  }


  if( xIS_SET( ch->act, PLR_AUTOGOLD ) && members < 2 )
    return;

  if( members < 2 )
  {
    send_to_char( "Just keep it all.\n\r", ch );
    return;
  }

  share = amount / members;
  extra = amount % members;

  if( share == 0 )
  {
    send_to_char( "Don't even bother, cheapskate.\n\r", ch );
    return;
  }

  ch->gold -= amount;
  ch->gold += share + extra;

  set_char_color( AT_GOLD, ch );
  ch_printf( ch, "You split %d zeni.  Your share is %d zeni.\n\r", amount, share + extra );

  sprintf( buf, "$n splits %d zeni.  Your share is %d zeni.", amount, share );

  for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
  {
    if( gch != ch && is_same_group( gch, ch ) )
    {
      act( AT_GOLD, buf, ch, NULL, gch, TO_VICT );
      gch->gold += share;
    }
  }
  return;
}



void do_gtell( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *gch;
#ifndef SCRAMBLE

  int speaking = -1, lang;

  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( ch->speaking & lang_array[lang] )
    {
      speaking = lang;
      break;
    }
#endif

  if( argument[0] == '\0' )
  {
    send_to_char( "Tell your group what?\n\r", ch );
    return;
  }

  if( xIS_SET( ch->act, PLR_NO_TELL ) )
  {
    send_to_char( "Your message didn't get through!\n\r", ch );
    return;
  }

  /*
   * Note use of send_to_char, so gtell works on sleepers.
   */
  /*
   * sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );
   */
  for( gch = first_char; gch; gch = gch->next )
  {
    if( is_same_group( gch, ch ) )
    {
      set_char_color( AT_GTELL, gch );
      /*
       * Groups unscrambled regardless of clan language.  Other languages
       * still garble though. -- Altrag 
       */
#ifndef SCRAMBLE

      if( speaking != -1 && ( !IS_NPC( ch ) || ch->speaking ) )
      {
        int speakswell = UMIN( knows_language( gch, ch->speaking, ch ),
                               knows_language( ch, ch->speaking, gch ) );

        if( speakswell < 85 )
          ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name,
                     translate( speakswell, argument, lang_names[speaking] ) );
        else
          ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, argument );
      }
      else
        ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, argument );
#else

      if( knows_language( gch, ch->speaking, gch ) || ( IS_NPC( ch ) && !ch->speaking ) )
        ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, argument );
      else
        ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, scramble( argument, ch->speaking ) );
#endif

    }
  }

  return;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA * ach, CHAR_DATA * bch )
{
  if( ach->leader )
    ach = ach->leader;
  if( bch->leader )
    bch = bch->leader;
  return ach == bch;
}

/*
 * this function sends raw argument over the AUCTION: channel
 * I am not too sure if this method is right..
 */

void talk_auction( char *argument )
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *original;

  sprintf( buf, "Auction: %s", argument );  /* last %s to reset color */

  for( d = first_descriptor; d; d = d->next )
  {
    original = d->original ? d->original : d->character;  /* if switched */
    if( ( d->connected == CON_PLAYING ) && !xIS_SET( original->deaf, CHANNEL_AUCTION )
        && !xIS_SET( original->in_room->room_flags, ROOM_SILENCE ) && !NOT_AUTHED( original )
        && ( ( original->pcdata->auction_pl <= auction->item->level && original != auction->seller )
             || original == auction->seller ) )
    {
      original->desc->psuppress_channel++;
      set_char_color( AT_CYAN, original );
      send_to_char( buf, original );
      send_to_char( "\n\r", original );
      //            act( AT_GOSSIP, buf, original, NULL, NULL, TO_CHAR );
    }
  }
}

/*
 * Language support functions. -- Altrag
 * 07/01/96
 *
 * Modified to return how well the language is known 04/04/98 - Thoric
 * Currently returns 100% for known languages... but should really return
 * a number based on player's wisdom (maybe 50+((25-wisdom)*2) ?)
 */
int knows_language( CHAR_DATA * ch, int language, CHAR_DATA * cch )
{
  sh_int sn;

  if( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    return 100;
  if( IS_NPC( ch ) && !ch->speaks ) /* No langs = knows all for npcs */
    return 100;
  if( IS_NPC( ch ) && IS_SET( ch->speaks, ( language & ~LANG_CLAN ) ) )
    return 100;
  /*
   * everyone KNOWS common tongue 
   */
  if( IS_SET( language, LANG_COMMON ) )
    return 100;
  if( language & LANG_CLAN )
  {
    /*
     * Clan = common for mobs.. snicker.. -- Altrag 
     */
    if( IS_NPC( ch ) || IS_NPC( cch ) )
      return 100;
    if( ch->pcdata->clan == cch->pcdata->clan && ch->pcdata->clan != NULL )
      return 100;
  }
  if( !IS_NPC( ch ) )
  {
    int lang;

    /*
     * Racial languages for PCs 
     */
    if( IS_SET( race_table[ch->race]->language, language ) )
      return 100;

    for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( IS_SET( language, lang_array[lang] ) && IS_SET( ch->speaks, lang_array[lang] ) )
      {
        if( ( sn = skill_lookup( lang_names[lang] ) ) != -1 )
          return ch->pcdata->learned[sn];
      }
  }
  return 0;
}

bool can_learn_lang( CHAR_DATA * ch, int language )
{
  if( language & LANG_CLAN )
    return FALSE;
  if( IS_NPC( ch ) || IS_IMMORTAL( ch ) )
    return FALSE;
  if( race_table[ch->race]->language & language )
    return FALSE;
  if( ch->speaks & language )
  {
    int lang;

    for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
      if( language & lang_array[lang] )
      {
        int sn;

        if( !( VALID_LANGS & lang_array[lang] ) )
          return FALSE;
        if( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
        {
          bug( "Can_learn_lang: valid language without sn: %d", lang );
          continue;
        }
        if( ch->pcdata->learned[sn] >= 99 )
          return FALSE;
      }
  }
  if( VALID_LANGS & language )
    return TRUE;
  return FALSE;
}

int const lang_array[] = {
  LANG_COMMON, LANG_SAIYAN, LANG_HALFBREED, LANG_NAMEK,
  LANG_ANDROID, LANG_ICER, LANG_BIO_ANDROID, LANG_KAIO,
  LANG_DEMON, LANG_WIZARD,
  LANG_CLAN, LANG_UNKNOWN
};

char *const lang_names[] = {
  "common", "saiyan", "halfbreed", "namek", "android",
  "icer", "bio-android", "kaio", "demon",
  "wizard", "clan", ""
};


/* Note: does not count racial language.  This is intentional (for now). */
int countlangs( int languages )
{
  int numlangs = 0;
  int looper;

  for( looper = 0; lang_array[looper] != LANG_UNKNOWN; looper++ )
  {
    if( lang_array[looper] == LANG_CLAN )
      continue;
    if( languages & lang_array[looper] )
      numlangs++;
  }
  return numlangs;
}

void do_speak( CHAR_DATA * ch, char *argument )
{
  int langs;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument( argument, arg );

  if( !str_cmp( arg, "all" ) && IS_IMMORTAL( ch ) )
  {
    set_char_color( AT_SAY, ch );
    ch->speaking = ~LANG_CLAN;
    send_to_char( "Now speaking all languages.\n\r", ch );
    return;
  }
  for( langs = 0; lang_array[langs] != LANG_UNKNOWN; langs++ )
    if( !str_prefix( arg, lang_names[langs] ) )
      if( knows_language( ch, lang_array[langs], ch ) )
      {
        if( lang_array[langs] == LANG_CLAN && ( IS_NPC( ch ) || !ch->pcdata->clan ) )
          continue;
        ch->speaking = lang_array[langs];
        set_char_color( AT_SAY, ch );
        ch_printf( ch, "You now speak %s.\n\r", lang_names[langs] );
        return;
      }
  set_char_color( AT_SAY, ch );
  send_to_char( "You do not know that language.\n\r", ch );
}

void do_languages( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  int lang;

  argument = one_argument( argument, arg );
  if( arg[0] != '\0' && !str_prefix( arg, "learn" ) && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
  {
    CHAR_DATA *sch;
    char arg2[MAX_INPUT_LENGTH];
    int sn;
    int prct;
    int prac;

    argument = one_argument( argument, arg2 );
    if( arg2[0] == '\0' )
    {
      send_to_char( "Learn which language?\n\r", ch );
      return;
    }
    for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    {
      if( lang_array[lang] == LANG_CLAN )
        continue;
      if( !str_prefix( arg2, lang_names[lang] ) )
        break;
    }
    if( lang_array[lang] == LANG_UNKNOWN )
    {
      send_to_char( "That is not a language.\n\r", ch );
      return;
    }
    if( !( VALID_LANGS & lang_array[lang] ) )
    {
      send_to_char( "You may not learn that language.\n\r", ch );
      return;
    }
    if( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
    {
      send_to_char( "That is not a language.\n\r", ch );
      return;
    }
    if( race_table[ch->race]->language & lang_array[lang] ||
        lang_array[lang] == LANG_COMMON || ch->pcdata->learned[sn] >= 99 )
    {
      act( AT_PLAIN, "You are already fluent in $t.", ch, lang_names[lang], NULL, TO_CHAR );
      return;
    }
    for( sch = ch->in_room->first_person; sch; sch = sch->next_in_room )
      if( IS_NPC( sch ) && xIS_SET( sch->act, ACT_SCHOLAR )
          && knows_language( sch, ch->speaking, ch )
          && knows_language( sch, lang_array[lang], sch ) && ( !sch->speaking || knows_language( ch, sch->speaking, sch ) ) )
        break;
    if( !sch )
    {
      send_to_char( "There is no one who can teach that language here.\n\r", ch );
      return;
    }
    if( countlangs( ch->speaks ) >= ( ch->level / 10 ) && ch->pcdata->learned[sn] <= 0 )
    {
      act( AT_TELL, "$n tells you 'You may not learn a new language yet.'", sch, NULL, ch, TO_VICT );
      return;
    }
    /*
     * 0..16 cha = 2 pracs, 17..25 = 1 prac. -- Altrag 
     */
    prac = 2;
    if( ch->practice < prac )
    {
      act( AT_TELL, "$n tells you 'You do not have enough practices.'", sch, NULL, ch, TO_VICT );
      return;
    }
    ch->practice -= prac;
    /*
     * Max 12% (5 + 4 + 3) at 24+ int and 21+ wis. -- Altrag 
     */
    prct = 5 + ( get_curr_int( ch ) / 30 );
    ch->pcdata->learned[sn] += prct;
    ch->pcdata->learned[sn] = UMIN( ch->pcdata->learned[sn], 99 );
    SET_BIT( ch->speaks, lang_array[lang] );
    if( ch->pcdata->learned[sn] == prct )
      act( AT_PLAIN, "You begin lessons in $t.", ch, lang_names[lang], NULL, TO_CHAR );
    else if( ch->pcdata->learned[sn] < 60 )
      act( AT_PLAIN, "You continue lessons in $t.", ch, lang_names[lang], NULL, TO_CHAR );
    else if( ch->pcdata->learned[sn] < 60 + prct )
      act( AT_PLAIN, "You feel you can start communicating in $t.", ch, lang_names[lang], NULL, TO_CHAR );
    else if( ch->pcdata->learned[sn] < 99 )
      act( AT_PLAIN, "You become more fluent in $t.", ch, lang_names[lang], NULL, TO_CHAR );
    else
      act( AT_PLAIN, "You now speak perfect $t.", ch, lang_names[lang], NULL, TO_CHAR );
    return;
  }
  for( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
    if( knows_language( ch, lang_array[lang], ch ) )
    {
      if( ch->speaking & lang_array[lang] || ( IS_NPC( ch ) && !ch->speaking ) )
        set_char_color( AT_SAY, ch );
      else
        set_char_color( AT_PLAIN, ch );
      send_to_char( lang_names[lang], ch );
      send_to_char( "\n\r", ch );
    }
  send_to_char( "\n\r", ch );
  return;
}

void do_traffic( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_TRAFFIC, "openly traffic" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_wartalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_WARTALK, "WAR" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

void do_racetalk( CHAR_DATA * ch, char *argument )
{
  if( NOT_AUTHED( ch ) )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if( !IS_IMMORTAL( ch ) && ch->exp < 5000 )
  {
    send_to_char( "You can't use this channel until you are out of training.\n\r", ch );
    return;
  }
  if( is_swear( argument ) )
  {
    send_to_char( "Oops, you shouldn't be swearing!\n\r", ch );
    send_to_char( "The word caught was: ", ch );
    send_to_char( is_swear( argument ), ch );
    send_to_char( "\n\r", ch );

    return;
  }
  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, argument, CHANNEL_RACETALK, "RACETALK" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}


#ifdef PROFANITY_CHECK
void init_profanity_checker(  )
{
#ifndef WIN32
  bigregex = ( char * )malloc( 4096 );
  add_profane_word( "fuck" );
  /*
   * skip over first 2 slashes and bar 
   */
  bigregex += 3;

  add_profane_word( "shit" );
  add_profane_word( "cunt" );

  preg = re_comp( bigregex );
#endif
}
#endif

void add_profane_word( char *word )
{
#ifndef WIN32
  char _word[4096];
  int i, j;

  j = 0;
  _word[j] = '\\';
  j++;
  _word[j] = '|';
  j++;
  _word[j] = '\\';
  j++;
  _word[j] = '(';
  j++;
  for( i = 0; i < strlen( word ); i++ )
  {
    _word[j] = '[';
    j++;
    _word[j] = tolower( word[i] );
    j++;
    _word[j] = toupper( word[i] );
    j++;
    _word[j] = ']';
    j++;
    _word[j] = '+';
    j++;
    _word[j] = '[';
    j++;



    _word[j] = '-';
    j++;
    _word[j] = ' ';
    j++;
    _word[j] = '\t';
    j++;

    _word[j] = '`';
    j++;
    _word[j] = '~';
    j++;
    _word[j] = '1';
    j++;
    _word[j] = '!';
    j++;
    _word[j] = '2';
    j++;
    _word[j] = '@';
    j++;
    _word[j] = '3';
    j++;
    _word[j] = '#';
    j++;
    _word[j] = '4';
    j++;
    _word[j] = '5';
    j++;
    _word[j] = '%';
    j++;
    _word[j] = '6';
    j++;
    _word[j] = '7';
    j++;
    _word[j] = '&';
    j++;
    _word[j] = '8';
    j++;
    _word[j] = '9';
    j++;
    _word[j] = '0';
    j++;
    _word[j] = '_';
    j++;
    _word[j] = ';';
    j++;
    _word[j] = ':';
    j++;
    _word[j] = ',';
    j++;
    _word[j] = '<';
    j++;
    /*
     * These need to be escaped  for C 
     */


    _word[j] = '\'';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '\"';
    j++;

    /*
     * These need to be escaped  for regex
     */
    _word[j] = '\\';
    j++;
    _word[j] = '$';
    j++;

    _word[j] = '>';
    j++;
    _word[j] = '/';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '^';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '.';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = ')';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '?';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '*';
    j++;

    _word[j] = '\\';
    j++;
    _word[j] = '(';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '[';
    j++;

    _word[j] = '\\';
    j++;
    _word[j] = '{';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '+';
    j++;

#ifdef BIG
    /*
     * i don't get what the deal is with this guy, it seems unescapable,
     * so to speak. 
     */
    _word[j] = '\\';
    j++;
    _word[j] = ']';
    j++;
#endif

    _word[j] = '\\';
    j++;
    _word[j] = '}';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '|';
    j++;
    _word[j] = '\\';
    j++;
    _word[j] = '=';
    j++;

    /*
     * close up funny characters 
     */
    _word[j] = ']';
    j++;
    _word[j] = '*';
    j++;
  }
  _word[j] = '\\';
  j++;
  _word[j] = ')';
  j++;
  _word[j] = '\0';

  strcat( bigregex, _word );
#endif
}

int is_profane( char *what )
{
  /*
   * #ifndef WIN32
   * int ret;
   * 
   * ret = re_exec(what);
   * if (ret==1)
   * return(1);
   * #endif
   */
  return ( 0 );
}

/*Add into file about line 604*/
/*cbusch was here*/
/* Chat bot disabled because it doesn't compile right
 * and it sucked in the first place :) -Goku 10.14.03 */
void chatperform( CHAR_DATA * ch, CHAR_DATA * victim, char *msg, int trigfrom )
{
  return;
}


/*about line 637*/
/*cbusch was here*/
void chatperformtoroom( char *txt, CHAR_DATA * ch )
{
  return;
}

/* Code from Snake/Zyon of DBS:R 
 * Aka fullsave command -Karn
 * Originally Modified by: Chosen(DBI)
 */
void do_fullsave( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  AREA_DATA *tarea;
  char filename[256];

  /*
   * Save the characters 
   */
  for( victim = first_char; victim; victim = victim->next )
  {
    save_char_obj( victim );
  }
  /*
   * Build areas 
   */
  for( tarea = first_build; tarea; tarea = tarea->next )
  {
    if( !IS_SET( tarea->status, AREA_LOADED ) )
    {
      continue;
    }
    sprintf( filename, "%s%s", BUILD_DIR, tarea->filename );
    fold_area( tarea, filename, FALSE );
  }
  rename( "help.are", "help.are.bak" );
  do_hset( ch, "save" );
  //do_chsave( ch, "save" ); 
  //do_cedit( ch, "save cmdtable" ); 
  save_classes(  );
  save_commands(  );
  save_socials(  );
  save_skill_table(  );
  save_herb_table(  );
  send_to_char( "All game data saved.\n\r", ch );
}

/*
 * Channel to allow admin to talk directly to players without
 * having to use tell and so that the other admin can also see
 * it. -- Melora
 */
void do_adminto( CHAR_DATA * ch, char *argument )
{
  char origarg[MAX_STRING_LENGTH];
  char charName[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  if( IS_NPC( ch ) )
  {
    pager_printf_color( ch, "NPC's can't use the ADMIN channel." );
    return;
  }

  sprintf( origarg, "%s", argument );
  argument = one_argument( argument, charName );
  if( ( victim = get_char_world( ch, charName ) ) == NULL )
  {
    send_to_char( "No one with that name found.\n\r", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "You don't need to use this with NPCs!\n\r", ch );
    return;
  }

  undeaf_admin(  );

  sysdata.outBytesFlag = LOGBOUTCHANNEL;
  talk_channel( ch, origarg, CHANNEL_ADMTO, "ADMINTO" );
  sysdata.outBytesFlag = LOGBOUTNORM;
  return;
}

char *colored_race_name( CHAR_DATA * ch )
{
  if( !IS_NPC( ch ) )
  {
    if(IS_BAAL(ch))
      return ("&bR&Be&cp&Clifo&cr&Bc&be C&Bo&cm&Cma&cn&Bd&bo&D");
    else if(IS_KAL(ch))
      return ("&zL&Wi&zg&We&zr &WZ&ze&Wr&zo&G: &RO&zm&re&zg&Ra&D");
    else if(IS_REM(ch))
      return ("&RS&Ca&Ri&Cy&Ra&Cn &RR&Ce&Ra&Cp&Re&Cr&D");
    else if(IS_ROLLZ(ch))
      return ("&BMe&Cc&ch&wan&ci&Cc&Bal &PG&Wo&pd&Pd&We&ps&Ps&d");
    else if(IS_KARRIE(ch))
      return ("&PK&pa&Pl&p'&Ps &pW&Po&pm&Pa&pn&D");
    else if(IS_AIZEN(ch))
      return ("&RE&rs&Rp&ra&Rd&ra&D");
    else if(IS_RINI2(ch))
      return ("&CD&ci&Wvi&cn&Ce &WR&Ca&wd&cia&wn&Cc&We&D");
    else if(IS_RINI(ch))
      return ("&YL&Oe&Yg&Oe&Yn&Od&Ya&Or&Yy &RS&ru&Rp&re&Rr &CS&ca&wi&Cy&ca&wn&D");
    else if(IS_SOPHIA(ch))
      return ("&WA&wn&Wg&we&Wl&wl&Wi&wc &WB&we&Wa&wu&Wt&wy&D");
    else if(IS_NATALIA(ch))
      return ("&BU&rc&wh&Bi&rh&wa&D");
    else if(IS_MALIK(ch))
      return ("&RD&re&Rs&ro&Rl&ra&Rt&re &BD&br&Be&ba&Bm&bs&D");
    else if(IS_SHOMA(ch))
      return ("}GD}zr}W. }wD}gi}wc}Wk}zw}Ge}ze}Wd&D");
    else if(IS_NICCI(ch))
      return ("&BS&bq&Bu&ba&Bd &bL&Be&ba&Bd&be&Br&D");
    else if(IS_IRONMAN(ch))
      return ("&YI&rr&Yo&rn&Ym&ra&Yn&D");
    else if(IS_AUSTIN(ch))
      return ("}OS}zh}Re}rl}Ol}z B}Ru}rl}Ol}ze}Rt&D");
    else if(IS_SIDEON(ch))
      return ("&GD&za&gi&zm&Ga&zo&gu &RR&We&zb&Ro&Wr&zn&D");
    else if(IS_BLKMAGE(ch))
      return ("&bB&Bl&za&Bc&bk &zM&Ba&bg&ze&D");
    else if(IS_MATT(ch))
      return ("&zDe&Wan&z's &CI&cc&Ce&cd &YT&Oe&Ya&D");
    else if(IS_ICHIGO(ch))
      return ("&RV&ri&Rz&ra&Rr&rd&D");
    else if(IS_DOMINIC(ch))
      return ("&RA&rn&Ro&rm&Ra&rl&Ry&D");
    else if(IS_ALTHAS(ch))
      return ("&BS&bt&po&br&Bm&D");
    else if(IS_ANNOYING(ch))
      return ("&wD&zark &RF&rlame&D");
    else if(IS_SCAPEGOAT(ch))
      return ("&CS&cc&Ca&cp&Ce&RG&ro&Ra&rt&D");
    else if( IS_IMMORTAL( ch ) )
      return ( "&RA&rd&Rm&ri&Rn&ri&Rs&rt&Rr&ra&Rt&ro&Rr" );
    else if( is_namek( ch ) )
      return ( "&GN&ga&wm&ge&Gk" );
    else if( is_saiyan( ch ) )
      return ( "&CS&ca&wi&cy&Ca&wn" );
    else if( is_hb( ch ) )
      return ( "&RH&ra&Wlf&rb&Rr&re&Wed" );
    else if( is_android( ch ) )
      return ( "&zA&wn&zd&wr&zo&wi&zd" );
    else if( IS_DEMON( ch ) )
      return ( "&rD&ze&rm&zo&rn" );
    else if( IS_MUTANT( ch ) )
      return ( "&GM&gu&Pt&pa&Gn&gt&D" );
    else if( is_human( ch ) )
      return ( "&wH&Wu&wm&Wa&wn" );
    else if( is_bio( ch ) )
      return ( "&gB&zi&go&z-&gA&zn&gd&zr&go&zi&gd" );
    else if( IS_CANDYMAN( ch ) )
      return ( "&PC&pa&Pn&pd&Py &pM&Pa&pn&D" );
    else if( is_genie( ch ) )
      return ( "&PM&pa&Pj&pi&Pn &pG&Pe&pn&Pi&pe" );
    else if( IS_KAIO( ch ) )
      return ( "&WK&Ca&ci&Wo&Cs&ch&Wi&Cn" );
    else if( is_saibaman( ch ) )
      return ( "&GSa&gib&wa&gma&Gn" );
    else if( IS_ICER( ch ) )
      return ( "&PI&pc&Pe&pr" );
    else if( is_fierian( ch ) )
      return ( "&RF&zi&Re&zr&Ri&za&Rn" );
	else if( is_hylian( ch ) )
      return ( "&GH&Yy&Gl&Yi&Ga&Yn" );  
	else if( is_reploid( ch ) )
      return ( "&BR&Ce&cp&wl&co&Ci&Bd" ); 
	else if( IS_NEPHALIM( ch ) )
      return ( "&RN&re&zp&wh&za&Wl&ri&Rm&D" );
	else if( IS_ARCHANGEL( ch ) )
      return ( "&CA&wr&Cc&Wh&CA&wn&Cg&We&Cl&D" );
	else if( IS_ARCHDEMON( ch ) )
      return ( "&RA&zr&rc&zh&RD&ze&rm&zo&Rn&D" );
	else if( is_dragon( ch ) )
      return ( "&zD&Rr&Yag&Ro&zn&D" );
	else
      return ( get_race( ch ) );
  }
  else
    return ( "&RUNKNOWN&D" );}

