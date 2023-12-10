#ifndef _COLOR_CODES_H_
#define _COLOR_CODES_H_

// =========================== Color Codes ============================
//Reset
#define RESET_COLOR "\e[0m"

//Regular text
#define BLK_COLOR "\e[0;30m"
#define RED_COLOR "\e[0;31m"
#define GRN_COLOR "\e[0;32m"
#define YEL_COLOR "\e[0;33m"
#define BLU_COLOR "\e[0;34m"
#define MAG_COLOR "\e[0;35m"
#define CYN_COLOR "\e[0;36m"
#define WHT_COLOR "\e[0;37m"
#define ORG_COLOR "\e[38;2;255;85;0m"
#define PNK_COLOR "\e[38;2;255;182;193m"

#define BLACK(str)      BLK_COLOR str RESET_COLOR
#define RED(str)        RED_COLOR str RESET_COLOR
#define GREEN(str)      GRN_COLOR str RESET_COLOR
#define YELLOW(str)     YEL_COLOR str RESET_COLOR
#define BLUE(str)       BLU_COLOR str RESET_COLOR
#define MAGANTA(str)    MAG_COLOR str RESET_COLOR
#define CYAN(str)       CYN_COLOR str RESET_COLOR
#define WHITE(str)      WHT_COLOR str RESET_COLOR
#define ORANGE(str)     ORG_COLOR str RESET_COLOR
#define PINK(str)       PNK_COLOR str RESET_COLOR

//Regular bold text
#define BBLK_COLOR "\e[1;30m"
#define BRED_COLOR "\e[1;31m"
#define BGRN_COLOR "\e[1;32m"
#define BYEL_COLOR "\e[1;33m"
#define BBLU_COLOR "\e[1;34m"
#define BMAG_COLOR "\e[1;35m"
#define BCYN_COLOR "\e[1;36m"
#define BWHT_COLOR "\e[1;37m"

#define BBLACK(str)     BBLK_COLOR str RESET_COLOR
#define BRED(str)       BRED_COLOR str RESET_COLOR
#define BGREEN(str)     BGRN_COLOR str RESET_COLOR
#define BYELLOW(str)    BYEL_COLOR str RESET_COLOR
#define BBLUE(str)      BBLU_COLOR str RESET_COLOR
#define BMAGANTA(str)   BMAG_COLOR str RESET_COLOR
#define BCYAN(str)      BCYN_COLOR str RESET_COLOR
#define BWHITE(str)     BWHT_COLOR str RESET_COLOR

//Regular underline text
#define UBLK_COLOR "\e[4;30m"
#define URED_COLOR "\e[4;31m"
#define UGRN_COLOR "\e[4;32m"
#define UYEL_COLOR "\e[4;33m"
#define UBLU_COLOR "\e[4;34m"
#define UMAG_COLOR "\e[4;35m"
#define UCYN_COLOR "\e[4;36m"
#define UWHT_COLOR "\e[4;37m"

#define UBLACK(str)     UBLK_COLOR str RESET_COLOR
#define URED(str)       URED_COLOR str RESET_COLOR
#define UGREEN(str)     UGRN_COLOR str RESET_COLOR
#define UYELLOW(str)    UYEL_COLOR str RESET_COLOR
#define UBLUE(str)      UBLU_COLOR str RESET_COLOR
#define UMAGANTA(str)   UMAG_COLOR str RESET_COLOR
#define UCYAN(str)      UCYN_COLOR str RESET_COLOR
#define UWHITE(str)     UWHT_COLOR str RESET_COLOR

//Regular background
#define BLKB_COLOR "\e[40m"
#define REDB_COLOR "\e[41m"
#define GRNB_COLOR "\e[42m"
#define YELB_COLOR "\e[43m"
#define BLUB_COLOR "\e[44m"
#define MAGB_COLOR "\e[45m"
#define CYNB_COLOR "\e[46m"
#define WHTB_COLOR "\e[47m"

#define BLACKB(str)     BLKB_COLOR str RESET_COLOR
#define REDB(str)       REDB_COLOR str RESET_COLOR
#define GREENB(str)     GRNB_COLOR str RESET_COLOR
#define YELLOWB(str)    YELB_COLOR str RESET_COLOR
#define BLUEB(str)      BLUB_COLOR str RESET_COLOR
#define MAGANTAB(str)   MAGB_COLOR str RESET_COLOR
#define CYANB(str)      CYNB_COLOR str RESET_COLOR
#define WHITEB(str)     WHTB_COLOR str RESET_COLOR

//High intensty background 
#define BLKHB_COLOR "\e[0;100m"
#define REDHB_COLOR "\e[0;101m"
#define GRNHB_COLOR "\e[0;102m"
#define YELHB_COLOR "\e[0;103m"
#define BLUHB_COLOR "\e[0;104m"
#define MAGHB_COLOR "\e[0;105m"
#define CYNHB_COLOR "\e[0;106m"
#define WHTHB_COLOR "\e[0;107m"

#define BLACKHB(str)    BLKHB_COLOR str RESET_COLOR
#define REDHB(str)      REDHB_COLOR str RESET_COLOR
#define GREENHB(str)    GRNHB_COLOR str RESET_COLOR
#define YELLOWHB(str)   YELHB_COLOR str RESET_COLOR
#define BLUEHB(str)     BLUHB_COLOR str RESET_COLOR
#define MAGANTAHB(str)  MAGHB_COLOR str RESET_COLOR
#define CYANHB(str)     CYNHB_COLOR str RESET_COLOR
#define WHITEHB(str)    WHTHB_COLOR str RESET_COLOR

//High intensty text
#define HBLK_COLOR "\e[0;90m"
#define HRED_COLOR "\e[0;91m"
#define HGRN_COLOR "\e[0;92m"
#define HYEL_COLOR "\e[0;93m"
#define HBLU_COLOR "\e[0;94m"
#define HMAG_COLOR "\e[0;95m"
#define HCYN_COLOR "\e[0;96m"
#define HWHT_COLOR "\e[0;97m"

#define HBLACK(str)     HBLK_COLOR str RESET_COLOR
#define HRED(str)       HRED_COLOR str RESET_COLOR
#define HGREEN(str)     HGRN_COLOR str RESET_COLOR
#define HYELLOW(str)    HYEL_COLOR str RESET_COLOR
#define HBLUE(str)      HBLU_COLOR str RESET_COLOR
#define HMAGANTA(str)   HMAG_COLOR str RESET_COLOR
#define HCYAN(str)      HCYN_COLOR str RESET_COLOR
#define HWHITE(str)     HWHT_COLOR str RESET_COLOR

//Bold high intensity text
#define BHBLK_COLOR "\e[1;90m"
#define BHRED_COLOR "\e[1;91m"
#define BHGRN_COLOR "\e[1;92m"
#define BHYEL_COLOR "\e[1;93m"
#define BHBLU_COLOR "\e[1;94m"
#define BHMAG_COLOR "\e[1;95m"
#define BHCYN_COLOR "\e[1;96m"
#define BHWHT_COLOR "\e[1;97m"

#define BHBLACK(str)    BHBLK_COLOR str RESET_COLOR
#define BHRED(str)      BHRED_COLOR str RESET_COLOR
#define BHGREEN(str)    BHGRN_COLOR str RESET_COLOR
#define BHYELLOW(str)   BHYEL_COLOR str RESET_COLOR
#define BHBLUE(str)     BHBLU_COLOR str RESET_COLOR
#define BHMAGANTA(str)  BHMAG_COLOR str RESET_COLOR
#define BHCYAN(str)     BHCYN_COLOR str RESET_COLOR
#define BHWHITE(str)    BHWHT_COLOR str RESET_COLOR

#endif