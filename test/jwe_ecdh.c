/* Public domain, no copyright. Use at your own risk. */

#include <stdio.h>

#include <check.h>
#include <yder.h>
#include <orcania.h>
#include <rhonabwy.h>

#define PAYLOAD "The true sign of intelligence is not knowledge but imagination..."

#if NETTLE_VERSION_NUMBER >= 0x030600

#define HUGE_DATA "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis efficitur lectus sit amet libero gravida eleifend. Nulla aliquam accumsan erat, quis tincidunt purus ultricies eu. Aenean eu dui ac diam placerat mollis. Duis eget tempor ipsum, vel ullamcorper purus. Ut eget quam vehicula, congue urna vel, dictum risus. Duis tristique est sed diam lobortis commodo. Proin et urna in odio malesuada sagittis. Donec lectus ligula, porttitor sed lorem ut, malesuada posuere neque. Nullam et nisl a felis congue mattis id non lectus.\
Quisque viverra hendrerit malesuada. Integer sollicitudin magna purus, in dignissim eros ullamcorper et. Praesent dignissim metus neque, eget tempor dolor tincidunt egestas. Nulla odio risus, tincidunt et egestas aliquet, pellentesque et eros. Etiam mattis orci a dui efficitur pharetra. Donec fermentum sem sed lacus finibus, nec luctus nisl vulputate. Donec sodales, nisi sed posuere maximus, lectus elit fermentum sapien, quis volutpat risus nisl vel dui. In vitae ante diam.\
Vivamus a nisl quam. Proin in lectus nunc. Aliquam condimentum tellus non feugiat aliquam. Nulla eu mi ligula. Proin auctor varius massa sed consectetur. Nulla et ligula pellentesque, egestas dui eu, gravida arcu. Maecenas vehicula feugiat tincidunt. Aenean sed sollicitudin ex. Cras luctus facilisis erat eu pharetra. Vestibulum interdum consequat tellus nec sagittis. Aliquam tincidunt eget lectus non bibendum. Mauris ut consectetur diam.\
Interdum et malesuada fames ac ante ipsum primis in faucibus. Sed lorem lectus, ullamcorper consectetur quam ut, pharetra consectetur diam. Suspendisse eu erat quis nunc imperdiet lacinia vitae id arcu. Fusce non euismod urna. Aenean lacinia porta tellus nec rutrum. Aliquam est magna, aliquam non hendrerit eget, scelerisque quis sapien. Quisque consectetur et lacus non dapibus. Duis diam purus, vulputate convallis faucibus in, rutrum quis mi. Sed sed magna eget tellus semper suscipit a in augue.\
Aenean vitae tortor quam. Praesent pulvinar nulla a nisi egestas, laoreet tempus mauris ullamcorper. Nam vulputate molestie velit, quis laoreet felis suscipit euismod. Pellentesque a enim dapibus, tincidunt lorem vel, suscipit turpis. Phasellus id metus vehicula, luctus sem nec, maximus purus. Duis dictum elit quam, quis rhoncus ex ullamcorper ut. Donec fringilla augue vitae vestibulum maximus. Mauris vel arcu eget arcu bibendum ornare."

#define TOKEN "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_HEADER "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMgo.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_DOTS "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_CIPHER_KEY "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.IEru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_IV "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPEQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_CIPHER "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FEbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_TAG "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PE8wi2GGHqzww"
#define TOKEN_INVALID_TAG_LEN "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGH"
#define TOKEN_INVALID_HEADER_B64 ";error;.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_CIPHER_KEY_B64 "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.;error;.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_IV_B64 "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.;error;.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_CIPHER_B64 "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.;error;.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_TAG_B64 "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.;error;"
#define TOKEN_OVERSIZE_APU "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eUNnIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19Cg.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_OVERSIZE_APV "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlaWEp5YjNKbGNuSnZjbVZ5Y205eVpYSnliM0psY25KdmNtVnljbTl5WlhKeWIzSmxjbkp2Y21WeWNtOXlDZyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19Cg.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_OVERSIZE_MISSING_EPK "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGt1Ijp7Imt0eSI6IkVDIiwieCI6IkFNd3JtNmp6aF9ndzNWZm9MWTROM0ppLUlOeWlPTTRGWG5yRzg4ajlHaHVhIiwieSI6IlJSTlBVd3ZYQ0VFd01YZjhJTnFvRmJ2MFh6WjU1RkhDd3RFeXBoUXhSMDAiLCJjcnYiOiJQLTI1NiJ9fQo.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_OVERSIZE_INVALID_EPK "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiY0d4dmNBbyIsInkiOiJaM0oxZEFvIiwiY3J2IjoiUC0yNTYifX0K.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_ENC "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExOTJDQkMtSFMzODQiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_APU "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiI7ZXJyb3I7IiwiYXB2IjoiWjNKMWRBbyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"
#define TOKEN_INVALID_APV "eyJhbGciOiJFQ0RILUVTK0ExMjhLVyIsImVuYyI6IkExMjhDQkMtSFMyNTYiLCJhcHUiOiJjR3h2Y0FvIiwiYXB2IjoiO2Vycm9yOyIsImtpZCI6IjEiLCJlcGsiOnsia3R5IjoiRUMiLCJ4IjoiQU13cm02anpoX2d3M1Zmb0xZNE4zSmktSU55aU9NNEZYbnJHODhqOUdodWEiLCJ5IjoiUlJOUFV3dlhDRUV3TVhmOElOcW9GYnYwWHpaNTVGSEN3dEV5cGhReFIwMCIsImNydiI6IlAtMjU2In19.Izru9wTpv5FPlPp7jpDZkueMZ3luMjXBaI2s0YgUtMiDPBAgXw8_GA.aKgPGQYvpPwHsQiDOeTFoQ.3syjxFimN-u5zY8t-mwIcZwVshIfYbzcxXID7FTbqdAKPWKlWfOdkXpk6V_u5p25U73Izv9qgr1UaWQAzaLli-LqFXptmCyciipYJc2BRhw.OwcQLpd3_PZ8wi2GGHqzww"

const char jwk_pubkey_ecdsa_str[] = "{\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4\","\
                                    "\"y\":\"4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM\",\"use\":\"enc\",\"kid\":\"1\",\"alg\":\"ES256\"}";
const char jwk_privkey_ecdsa_str[] = "{\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4\","\
                                     "\"y\":\"4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM\",\"d\":\"870MB6gfuTJ4HtUnUvYMyJpr5eUZNP4Bk43bVdj3eAE\","\
                                     "\"use\":\"enc\",\"kid\":\"1\",\"alg\":\"ES256\"}";
const char jwk_privkey_ecdsa_small_str[] = "{\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"cGxvcAo\","\
                                           "\"y\":\"Z3J1dAo\",\"d\":\"Y29pbgo\","\
                                           "\"use\":\"enc\",\"kid\":\"1\",\"alg\":\"ES256\"}";
const char jwk_privkey_ecdsa_large_str[] = "{\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"cz-Y7wdU6n3j6QB5J-KtVg1ozfir1yuRsWBMZ2NvZytGRj0V-41c92HxrsOC-Ia--0sK_ATpnRKlgS5jo286c6o\","\
                                           "\"y\":\"Adt3Ill4dNKRMdrDM4GYt-49GFmRcRYLiJveIzg1YkOduLHtoxQ4UQ5GpPZNfoJE3YBQkwoZIUKpG8WHKTHqYtM3\","\
                                           "\"d\":\"Aen_lIXMdZhFBSkgPdXGsyNTh6XEDDk74-08nawnxO1YblVM-Rp7FbumjBbCpA7jPxRxKO1h3VJFP93uvqw16RIE\","\
                                           "\"use\":\"enc\",\"kid\":\"1\",\"alg\":\"ES256\"}";
const char jwk_pubkey_ecdsa_small_str[] = "{\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"cGxvcAo\","\
                                          "\"y\":\"Z3J1dAo\",\"use\":\"enc\",\"kid\":\"1\",\"alg\":\"ES256\"}";
const char jwk_pubkey_ecdsa_large_str[] = "{\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"cz-Y7wdU6n3j6QB5J-KtVg1ozfir1yuRsWBMZ2NvZytGRj0V-41c92HxrsOC-Ia--0sK_ATpnRKlgS5jo286c6o\","\
                                          "\"y\":\"Adt3Ill4dNKRMdrDM4GYt-49GFmRcRYLiJveIzg1YkOduLHtoxQ4UQ5GpPZNfoJE3YBQkwoZIUKpG8WHKTHqYtM3\","\
                                          "\"use\":\"enc\",\"kid\":\"1\",\"alg\":\"ES256\"}";
const char jwk_privkey_ecdsa_p384_str[] = "{\"alg\":\"ES512\",\"crv\":\"P-384\",\"d\":\"IKRyzFYbvLo4JoBMlPPTdIE8mLlcCO03XLh-97aNT3sQ9cT4vb3nSjWCecTUvjW6\","\
                                          "\"kty\":\"EC\",\"x\":\"Nx2Wxf0o6A01m6ymiD7YIfmWkyBrySWzq5N85bMUs_9G8D_l4RRxLj4i7z1_5Rvo\","\
                                          "\"y\":\"c4iG6Gp5vEcoyuGREu40AejY-fkg0K_iYEic1sME-VphV8L94IWJU3EbhA_zEQhb\"}";
const char jwk_pubkey_ecdsa_p384_str[] = "{\"alg\":\"ES512\",\"crv\":\"P-384\",\"kty\":\"EC\","\
                                         "\"x\":\"Nx2Wxf0o6A01m6ymiD7YIfmWkyBrySWzq5N85bMUs_9G8D_l4RRxLj4i7z1_5Rvo\","\
                                         "\"y\":\"c4iG6Gp5vEcoyuGREu40AejY-fkg0K_iYEic1sME-VphV8L94IWJU3EbhA_zEQhb\"}";
const char jwk_privkey_ecdsa_p521_str[] = "{\"alg\":\"ES512\",\"crv\":\"P-521\",\"d\":\"cz-Y7wdU6n3j6QB5J-KtVg1ozfir1yuRsWBMZ2NvZytGRj0V-41c92HxrsOC-Ia--0sK_ATpnRKlgS5jo286c6o\","\
                                          "\"kty\":\"EC\",\"x\":\"Adt3Ill4dNKRMdrDM4GYt-49GFmRcRYLiJveIzg1YkOduLHtoxQ4UQ5GpPZNfoJE3YBQkwoZIUKpG8WHKTHqYtM3\","\
                                          "\"y\":\"Aen_lIXMdZhFBSkgPdXGsyNTh6XEDDk74-08nawnxO1YblVM-Rp7FbumjBbCpA7jPxRxKO1h3VJFP93uvqw16RIE\"}";
const char jwk_pubkey_ecdsa_p521_str[] = "{\"alg\":\"ES512\",\"crv\":\"P-521\",\"kty\":\"EC\","\
                                         "\"x\":\"Adt3Ill4dNKRMdrDM4GYt-49GFmRcRYLiJveIzg1YkOduLHtoxQ4UQ5GpPZNfoJE3YBQkwoZIUKpG8WHKTHqYtM3\","\
                                         "\"y\":\"Aen_lIXMdZhFBSkgPdXGsyNTh6XEDDk74-08nawnxO1YblVM-Rp7FbumjBbCpA7jPxRxKO1h3VJFP93uvqw16RIE\"}";
const char jwk_pubkey_ecdsa_str_2[] = "{\"kty\":\"EC\",\"x\":\"RKL0w34ppc4wuBuzotuWo9d6hGv59uWjgc5oimWQtYU\",\"y\":\"S8EabLKBmyT2v_vPSrpfWnYw6edRm9I60UQlbvSS1eU\","\
                                       "\"crv\":\"P-256\",\"kid\":\"2\",\"alg\":\"ES256\"}";
const char jwk_privkey_ecdsa_str_2[] = "{\"kty\":\"EC\",\"x\":\"RKL0w34ppc4wuBuzotuWo9d6hGv59uWjgc5oimWQtYU\",\"y\":\"S8EabLKBmyT2v_vPSrpfWnYw6edRm9I60UQlbvSS1eU\""\
                                      ",\"d\":\"KMRJaGpxVer0w9lMjIY_UrjC067tZdEJkL5eaiBVWi8\",\"crv\":\"P-256\",\"kid\":\"2\",\"alg\":\"ES256\"}";
const char jwk_pubkey_rsa_str[] = "{\"kty\":\"RSA\",\"n\":\"0vx7agoebGcQSuuPiLJXZptN9nndrQmbXEps2aiAFbWhM78LhWx4cbbfAAtVT86zwu1RK7aPFFxuhDR1L6tSoc_BJECPebWKRX"\
                                   "jBZCiFV4n3oknjhMstn64tZ_2W-5JsGY4Hc5n9yBXArwl93lqt7_RN5w6Cf0h4QyQ5v-65YGjQR0_FDW2QvzqY368QQMicAtaSqzs8KJZgnYb9c7d0zgdAZHzu6"\
                                   "qMQvRL5hajrn1n91CbOpbISD08qNLyrdkt-bFTWhAI4vMQFh6WeZu0fM4lFd2NcRwr3XPksINHaQ-G_xBniIqbw0Ls1jF44-csFCur-kEgU8awapJzKnqDKgw\""\
                                   ",\"e\":\"AQAB\",\"alg\":\"RS256\",\"kid\":\"2011-04-29\"}";
const char jwk_pubkey_x25519_str[] = "{\"kty\":\"OKP\",\"use\":\"enc\",\"crv\":\"X25519\",\"x\":\"AuQ7nbIvxilE4nzzRoS_C_cmpqMx-kcXNkcAyy46fWM\"}";
const char jwk_privkey_x25519_str[] = "{\"kty\":\"OKP\",\"d\":\"-NOCJItqI-R-AFsq1cLNLAIpfIf-otm7x2psH5EXJoo\","
                                      "\"use\":\"enc\",\"crv\":\"X25519\",\"x\":\"AuQ7nbIvxilE4nzzRoS_C_cmpqMx-kcXNkcAyy46fWM\"}";
const char jwk_pubkey_x25519_small_str[] = "{\"kty\":\"OKP\",\"use\":\"enc\",\"crv\":\"X25519\",\"x\":\"cGxvcAo\"}";
const char jwk_privkey_x25519_small_str[] = "{\"kty\":\"OKP\",\"d\":\"Y29pbgo\",\"use\":\"enc\",\"crv\":\"X25519\",\"x\":\"cGxvcAo\"}";
const char jwk_pubkey_x25519_large_str[] = "{\"kty\":\"OKP\",\"use\":\"enc\",\"crv\":\"X25519\",\"x\":\"W46m2SwV-XgAWMqvPQe0KLy_-0CsHhb5r6y11aj7bJBK1F2fvWg02iEsGd5JyA5A3qllofTJwoQ\"}";
const char jwk_privkey_x25519_large_str[] = "{\"kty\":\"OKP\",\"d\":\"DFFZ-8-3Q7xEBHV0VVC1JmBL4oMrRo9zDKqLIJF1GEJgNGgrBYY5CrsoZbgs6NOurHTp73o6jhM\","
                                            "\"use\":\"enc\",\"crv\":\"X25519\",\"x\":\"W46m2SwV-XgAWMqvPQe0KLy_-0CsHhb5r6y11aj7bJBK1F2fvWg02iEsGd5JyA5A3qllofTJwoQ\"}";
const char jwk_privkey_x25519_str_2[] = "{\"kty\":\"OKP\",\"d\":\"kcIdGcJVDgzC6KLd9I1P7of4RJvXxZZmilCh_f-0K8Q\","
                                        "\"use\":\"enc\",\"crv\":\"X25519\",\"x\":\"JIrudOxnjSYGNO6Jsa7Bp00juLU10XB6ZutgPgpfEyE\"}";
const char jwk_pubkey_x448_str[] = "{\"kty\":\"OKP\",\"use\":\"enc\",\"crv\":\"X448\",\"x\":\"W46m2SwV-XgAWMqvPQe0KLy_-0CsHhb5r6y11aj7bJBK1F2fvWg02iEsGd5JyA5A3qllofTJwoQ\"}";
const char jwk_privkey_x448_str[] = "{\"kty\":\"OKP\",\"d\":\"DFFZ-8-3Q7xEBHV0VVC1JmBL4oMrRo9zDKqLIJF1GEJgNGgrBYY5CrsoZbgs6NOurHTp73o6jhM\","
                                    "\"use\":\"enc\",\"crv\":\"X448\",\"x\":\"W46m2SwV-XgAWMqvPQe0KLy_-0CsHhb5r6y11aj7bJBK1F2fvWg02iEsGd5JyA5A3qllofTJwoQ\"}";
const char jwk_privkey_x448_str_2[] = "{\"kty\":\"OKP\",\"d\":\"k_-0MeUxtYskqQkpSxWCKMhLCVfDbhW5pMysvAF84v7C9RI9cm5imhkAMs3ngjXAqUlAnwmQtRI\","
                                      "\"use\":\"enc\",\"crv\":\"X448\",\"x\":\"sXZMHweV1nAKE5sZ-z8Sp-Sbd0dYXbzqpjGMwPHORP1K1gsKLaQvLSmy4yStLRVPGoTCW8IPqyw\"}";

static void test_encrypt_decrypt_ok(jwa_alg alg, jwa_enc enc) {
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Test alg %s, enc %s", r_jwa_alg_to_str(alg), r_jwa_enc_to_str(enc));
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe_decrypt, jwk_privkey, NULL), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, alg), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, enc), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  o_free(token);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}

static void test_decrypt_invalid_key(jwa_alg alg, jwa_enc enc) {
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Test invalid key alg %s, enc %s", r_jwa_alg_to_str(alg), r_jwa_enc_to_str(enc));
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_ecdsa_str_2), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, alg), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, enc), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  
  o_free(token);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}

START_TEST(test_rhonabwy_encrypt_decrypt_ok)
{
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A128CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A192CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A256CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A128GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A192GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A256GCM);

  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A128CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A192CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A256CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A128GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A192GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A256GCM);

  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A128CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A192CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A256CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A128GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A192GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A256GCM);

  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A128CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A192CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A256CBC);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A128GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A192GCM);
  test_encrypt_decrypt_ok(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A256GCM);
}
END_TEST

START_TEST(test_rhonabwy_encrypt_decrypt_x25519_ok)
{
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Test X25519");
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe_decrypt, jwk_privkey, NULL), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  o_free(token);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_encrypt_decrypt_x448_ok)
{
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Test X25519");
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_x448_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_x448_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe_decrypt, jwk_privkey, NULL), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  o_free(token);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_encrypt_invalid_parameters)
{
  jwe_t * jwe;
  jwk_t * jwk_pubkey;

  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", ";not a base64;"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, NULL, 0), NULL);
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", ";not a base64;"), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, NULL, 0), NULL);
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", ";not a base64;"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", ";not a base64;"), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, NULL, 0), NULL);
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", NULL), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", ";not a base64;"), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, NULL, 0), NULL);
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", ";not a base64;"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", NULL), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, NULL, 0), NULL);
  
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
}
END_TEST

START_TEST(test_rhonabwy_decrypt_invalid_key)
{
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A128CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A192CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A256CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A128GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A192GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES, R_JWA_ENC_A256GCM);

  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A128CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A192CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A256CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A128GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A192GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A128KW, R_JWA_ENC_A256GCM);

  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A128CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A192CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A256CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A128GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A192GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A192KW, R_JWA_ENC_A256GCM);

  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A128CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A192CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A256CBC);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A128GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A192GCM);
  test_decrypt_invalid_key(R_JWA_ALG_ECDH_ES_A256KW, R_JWA_ENC_A256GCM);
}
END_TEST

START_TEST(test_rhonabwy_decrypt_invalid_x25519_key)
{
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Test invalid key X25519");
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_x25519_str_2), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  
  o_free(token);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_decrypt_invalid_x448_key)
{
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Test invalid key X25519");
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_x448_str_2), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_x448_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  
  o_free(token);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_encrypt_invalid_key_type)
{
  jwe_t * jwe;
  jwk_t * jwk_pubkey, * jwk_privkey;
  
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_ecdsa_p521_str), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);

  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_rsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, jwk_pubkey, 0), NULL);
  r_jwk_free(jwk_pubkey);
  
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_p521_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, jwk_pubkey, 0), NULL);
  r_jwk_free(jwk_pubkey);
  
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, jwk_privkey, NULL), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, jwk_pubkey, 0), NULL);
  r_jwk_free(jwk_pubkey);
  
  r_jwk_free(jwk_privkey);
  r_jwe_free(jwe);
}
END_TEST

START_TEST(test_rhonabwy_parse_token_invalid)
{
  jwe_t * jwe_decrypt;
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_HEADER, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_HEADER_B64, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_CIPHER_KEY_B64, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_IV_B64, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_CIPHER_B64, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_TAG_B64, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_DOTS, 0), RHN_ERROR_PARAM);
  
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_decrypt_token_invalid)
{
  jwe_t * jwe_decrypt;
  jwk_t * jwk_privkey;
  
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_ecdsa_str), RHN_OK);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_IV, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_CIPHER, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_TAG, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_TAG_LEN, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_OVERSIZE_APU, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_OVERSIZE_APV, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_OVERSIZE_MISSING_EPK, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_PARAM);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_OVERSIZE_INVALID_EPK, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_ENC, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_APU, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_PARAM);
  r_jwe_free(jwe_decrypt);
  
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, TOKEN_INVALID_APV, 0), RHN_ERROR_PARAM);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, jwk_privkey, 0), RHN_ERROR_PARAM);
  r_jwe_free(jwe_decrypt);
  
  r_jwk_free(jwk_privkey);
}
END_TEST

START_TEST(test_rhonabwy_flood_ok)
{
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe_decrypt, jwk_privkey, NULL), RHN_OK);
  
  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128CBC), RHN_OK);
  
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  o_free(token);
  
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  o_free(token);
  
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  o_free(token);
  
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  o_free(token);
  
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_check_key_length_ecdh_es)
{
  jwe_t * jwe_enc_1, * jwe_enc_2, * jwe_dec_1, * jwe_dec_2;
  jwk_t * jwk1_pub, * jwk1_priv, * jwk2_pub, * jwk2_priv;
  char * token_1, * token_2;
  
  ck_assert_int_eq(r_jwk_init(&jwk1_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_priv), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_1), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_2), RHN_OK);

  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_pub, jwk_pubkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_priv, jwk_privkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_pub, jwk_pubkey_x448_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_priv, jwk_privkey_x448_str), RHN_OK);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_1, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_1, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_1, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_1 = r_jwe_serialize(jwe_enc_1, jwk1_pub, 0)), NULL);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_2, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_2, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_2, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_2 = r_jwe_serialize(jwe_enc_2, jwk2_pub, 0)), NULL);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk1_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk2_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_2);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk2_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk1_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec_2);
  
  r_jwk_free(jwk1_pub);
  r_jwk_free(jwk1_priv);
  r_jwk_free(jwk2_pub);
  r_jwk_free(jwk2_priv);
  r_jwe_free(jwe_enc_1);
  r_jwe_free(jwe_enc_2);
  r_free(token_1);
  r_free(token_2);
}
END_TEST

START_TEST(test_rhonabwy_check_key_length_ecdh_es_a256kw)
{
  jwe_t * jwe_enc_1, * jwe_enc_2, * jwe_dec_1, * jwe_dec_2;
  jwk_t * jwk1_pub, * jwk1_priv, * jwk2_pub, * jwk2_priv;
  char * token_1, * token_2;
  
  ck_assert_int_eq(r_jwk_init(&jwk1_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_priv), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_1), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_2), RHN_OK);

  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_pub, jwk_pubkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_priv, jwk_privkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_pub, jwk_pubkey_x448_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_priv, jwk_privkey_x448_str), RHN_OK);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_1, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_1, R_JWA_ALG_ECDH_ES_A256KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_1, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_1 = r_jwe_serialize(jwe_enc_1, jwk1_pub, 0)), NULL);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_2, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_2, R_JWA_ALG_ECDH_ES_A256KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_2, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_2 = r_jwe_serialize(jwe_enc_2, jwk2_pub, 0)), NULL);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk1_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk2_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_2);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk2_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk1_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec_2);
  
  r_jwk_free(jwk1_pub);
  r_jwk_free(jwk1_priv);
  r_jwk_free(jwk2_pub);
  r_jwk_free(jwk2_priv);
  r_jwe_free(jwe_enc_1);
  r_jwe_free(jwe_enc_2);
  r_free(token_1);
  r_free(token_2);
}
END_TEST

START_TEST(test_rhonabwy_check_key_type_ecdh_es)
{
  jwe_t * jwe_enc_1, * jwe_enc_2, * jwe_dec_1, * jwe_dec_2;
  jwk_t * jwk1_pub, * jwk1_priv, * jwk2_pub, * jwk2_priv;
  char * token_1, * token_2;
  
  ck_assert_int_eq(r_jwk_init(&jwk1_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_priv), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_1), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_2), RHN_OK);

  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_pub, jwk_pubkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_priv, jwk_privkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_pub, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_priv, jwk_privkey_ecdsa_str), RHN_OK);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_1, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_1, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_1, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_1 = r_jwe_serialize(jwe_enc_1, jwk1_pub, 0)), NULL);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_2, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_2, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_2, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_2 = r_jwe_serialize(jwe_enc_2, jwk2_pub, 0)), NULL);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk1_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk2_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_2);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk2_priv, 0), RHN_ERROR_PARAM);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk1_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec_2);
  
  r_jwk_free(jwk1_pub);
  r_jwk_free(jwk1_priv);
  r_jwk_free(jwk2_pub);
  r_jwk_free(jwk2_priv);
  r_jwe_free(jwe_enc_1);
  r_jwe_free(jwe_enc_2);
  r_free(token_1);
  r_free(token_2);
}
END_TEST

START_TEST(test_rhonabwy_check_key_type_ecdh_es_a256kw)
{
  jwe_t * jwe_enc_1, * jwe_enc_2, * jwe_dec_1, * jwe_dec_2;
  jwk_t * jwk1_pub, * jwk1_priv, * jwk2_pub, * jwk2_priv;
  char * token_1, * token_2;
  
  ck_assert_int_eq(r_jwk_init(&jwk1_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_priv), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_1), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_2), RHN_OK);

  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_pub, jwk_pubkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_priv, jwk_privkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_pub, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_priv, jwk_privkey_ecdsa_str), RHN_OK);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_1, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_1, R_JWA_ALG_ECDH_ES_A256KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_1, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_1 = r_jwe_serialize(jwe_enc_1, jwk1_pub, 0)), NULL);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_2, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_2, R_JWA_ALG_ECDH_ES_A256KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_2, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_2 = r_jwe_serialize(jwe_enc_2, jwk2_pub, 0)), NULL);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk1_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk2_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_2);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk2_priv, 0), RHN_ERROR_PARAM);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk1_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec_2);
  
  r_jwk_free(jwk1_pub);
  r_jwk_free(jwk1_priv);
  r_jwk_free(jwk2_pub);
  r_jwk_free(jwk2_priv);
  r_jwe_free(jwe_enc_1);
  r_jwe_free(jwe_enc_2);
  r_free(token_1);
  r_free(token_2);
}
END_TEST

START_TEST(test_rhonabwy_check_key_length_ecdh_es_ecdsa)
{
  jwe_t * jwe_enc_1, * jwe_enc_2, * jwe_dec_1, * jwe_dec_2;
  jwk_t * jwk1_pub, * jwk1_priv, * jwk2_pub, * jwk2_priv;
  char * token_1, * token_2;
  
  ck_assert_int_eq(r_jwk_init(&jwk1_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_priv), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_1), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_2), RHN_OK);

  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_pub, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_priv, jwk_privkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_pub, jwk_pubkey_ecdsa_p384_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_priv, jwk_privkey_ecdsa_p384_str), RHN_OK);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_1, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_1, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_1, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_1 = r_jwe_serialize(jwe_enc_1, jwk1_pub, 0)), NULL);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_2, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_2, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_2, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_ne((token_2 = r_jwe_serialize(jwe_enc_2, jwk2_pub, 0)), NULL);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk1_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk2_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec_2);
  
  ck_assert_ptr_ne((jwe_dec_1 = r_jwe_quick_parse(token_1, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_1, jwk2_priv, 0), RHN_ERROR_PARAM);
  r_jwe_free(jwe_dec_1);

  ck_assert_ptr_ne((jwe_dec_2 = r_jwe_quick_parse(token_2, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec_2, jwk1_priv, 0), RHN_ERROR_PARAM);
  r_jwe_free(jwe_dec_2);
  
  r_jwk_free(jwk1_pub);
  r_jwk_free(jwk1_priv);
  r_jwk_free(jwk2_pub);
  r_jwk_free(jwk2_priv);
  r_jwe_free(jwe_enc_1);
  r_jwe_free(jwe_enc_2);
  r_free(token_1);
  r_free(token_2);
}
END_TEST

START_TEST(test_rhonabwy_check_key_length_invalid_ecddsa_key)
{
  jwe_t * jwe_enc_1, * jwe_dec;
  jwk_t * jwk1_pub, * jwk1_priv, * jwk1_invalid_pub, * jwk1_invalid_priv, * jwk2_invalid_pub, * jwk2_invalid_priv;
  char * token;
  
  ck_assert_int_eq(r_jwk_init(&jwk1_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_invalid_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_invalid_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_invalid_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_invalid_priv), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_1), RHN_OK);

  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_pub, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_priv, jwk_privkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_invalid_pub, jwk_pubkey_ecdsa_small_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_invalid_priv, jwk_privkey_ecdsa_small_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_invalid_pub, jwk_pubkey_ecdsa_large_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_invalid_priv, jwk_privkey_ecdsa_large_str), RHN_OK);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_1, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_1, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_1, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe_enc_1, jwk1_invalid_pub, 0), NULL);
  ck_assert_ptr_eq(r_jwe_serialize(jwe_enc_1, jwk2_invalid_pub, 0), NULL);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe_enc_1, jwk1_pub, 0)), NULL);

  ck_assert_ptr_ne((jwe_dec = r_jwe_quick_parse(token, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec, jwk1_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec);

  ck_assert_ptr_ne((jwe_dec = r_jwe_quick_parse(token, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec, jwk1_invalid_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec);

  ck_assert_ptr_ne((jwe_dec = r_jwe_quick_parse(token, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec, jwk2_invalid_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec);

  r_jwk_free(jwk1_pub);
  r_jwk_free(jwk1_priv);
  r_jwk_free(jwk1_invalid_pub);
  r_jwk_free(jwk1_invalid_priv);
  r_jwk_free(jwk2_invalid_pub);
  r_jwk_free(jwk2_invalid_priv);
  r_jwe_free(jwe_enc_1);
  r_free(token);
}
END_TEST

START_TEST(test_rhonabwy_check_key_length_invalid_eddsa_key)
{
  jwe_t * jwe_enc_1, * jwe_dec;
  jwk_t * jwk1_pub, * jwk1_priv, * jwk1_invalid_pub, * jwk1_invalid_priv, * jwk2_invalid_pub, * jwk2_invalid_priv;
  char * token;
  
  ck_assert_int_eq(r_jwk_init(&jwk1_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_invalid_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk1_invalid_priv), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_invalid_pub), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk2_invalid_priv), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_enc_1), RHN_OK);

  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_pub, jwk_pubkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_priv, jwk_privkey_x25519_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_invalid_pub, jwk_pubkey_x25519_small_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk1_invalid_priv, jwk_privkey_x25519_small_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_invalid_pub, jwk_pubkey_x25519_large_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk2_invalid_priv, jwk_privkey_x25519_large_str), RHN_OK);

  ck_assert_int_eq(r_jwe_set_payload(jwe_enc_1, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_set_alg(jwe_enc_1, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe_enc_1, R_JWA_ENC_A256CBC), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe_enc_1, jwk1_invalid_pub, 0), NULL);
  ck_assert_ptr_eq(r_jwe_serialize(jwe_enc_1, jwk2_invalid_pub, 0), NULL);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe_enc_1, jwk1_pub, 0)), NULL);

  ck_assert_ptr_ne((jwe_dec = r_jwe_quick_parse(token, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec, jwk1_priv, 0), RHN_OK);
  r_jwe_free(jwe_dec);

  ck_assert_ptr_ne((jwe_dec = r_jwe_quick_parse(token, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec, jwk1_invalid_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec);

  ck_assert_ptr_ne((jwe_dec = r_jwe_quick_parse(token, R_PARSE_NONE, 0)), NULL);
  ck_assert_int_eq(r_jwe_decrypt(jwe_dec, jwk2_invalid_priv, 0), RHN_ERROR_INVALID);
  r_jwe_free(jwe_dec);

  r_jwk_free(jwk1_pub);
  r_jwk_free(jwk1_priv);
  r_jwk_free(jwk1_invalid_pub);
  r_jwk_free(jwk1_invalid_priv);
  r_jwk_free(jwk2_invalid_pub);
  r_jwk_free(jwk2_invalid_priv);
  r_jwe_free(jwe_enc_1);
  r_free(token);
}
END_TEST

START_TEST(test_rhonabwy_check_apu)
{
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  struct _o_datum apv = {0, NULL}, apu = {0, NULL};
  
  ck_assert_int_eq(o_base64_encode_alloc((const unsigned char *)HUGE_DATA, o_strlen(HUGE_DATA)-20, &apu), 1);
  apu.data[apu.size] = '\0';
  
  ck_assert_int_eq(o_base64_encode_alloc((const unsigned char *)HUGE_DATA, o_strlen(HUGE_DATA), &apv), 1);
  apv.data[apv.size] = '\0';
  
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe_decrypt, jwk_privkey, NULL), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A256CBC), RHN_OK);
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", (const char *)apu.data), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  o_free(token);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  ck_assert_int_eq(r_jwe_set_header_int_value(jwe, "apu", 42), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_ERROR_PARAM);
  o_free(token);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", ""), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_ERROR_PARAM);
  o_free(token);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", ";error;"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Z3J1dAo"), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, NULL, 0), NULL);
  
  o_free(apu.data);
  o_free(apv.data);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_check_apv)
{
  jwe_t * jwe, * jwe_decrypt;
  jwk_t * jwk_privkey, * jwk_pubkey;
  char * token = NULL;
  struct _o_datum apv = {0, NULL}, apu = {0, NULL};
  
  ck_assert_int_eq(o_base64_encode_alloc((const unsigned char *)HUGE_DATA, o_strlen(HUGE_DATA)-20, &apu), 1);
  apu.data[apu.size] = '\0';
  
  ck_assert_int_eq(o_base64_encode_alloc((const unsigned char *)HUGE_DATA, o_strlen(HUGE_DATA), &apv), 1);
  apv.data[apv.size] = '\0';
  
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe_decrypt), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_privkey, jwk_privkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_pubkey, jwk_pubkey_ecdsa_str), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, NULL, jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe_decrypt, jwk_privkey, NULL), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES_A128KW), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A256CBC), RHN_OK);
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", (const char *)apu.data), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_OK);
  o_free(token);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  ck_assert_int_eq(r_jwe_set_header_int_value(jwe, "apv", 42), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_ERROR_PARAM);
  o_free(token);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", ""), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  
  ck_assert_int_eq(r_jwe_parse(jwe_decrypt, token, 0), RHN_ERROR_PARAM);
  o_free(token);
  ck_assert_int_eq(r_jwe_decrypt(jwe_decrypt, NULL, 0), RHN_OK);
  ck_assert_int_eq(0, memcmp(jwe_decrypt->payload, PAYLOAD, jwe_decrypt->payload_len));
  
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", ";error;"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "cGxvcAo"), RHN_OK);
  ck_assert_ptr_eq(r_jwe_serialize(jwe, NULL, 0), NULL);
  
  o_free(apu.data);
  o_free(apv.data);
  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwe_free(jwe);
  r_jwe_free(jwe_decrypt);
}
END_TEST

START_TEST(test_rhonabwy_rfc_ok)
{
  const char eph[] = " {\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"gI0GAILBdu7T53akrFmMyGcsF3n5dO7MmwNBHKW5SV0\",\"y\":\"SLW_xSffzlPWrHEVI30DHM_4egVwt3NQqeUD7nMFpps\",\"d\":\"0_NxaRPUMQoAJt50Gz8YiTr8gRTwyEaCumd-MToTmIo\"}",
  bob[] = "{\"kty\":\"EC\",\"crv\":\"P-256\",\"x\":\"weNJy2HscCSM6AEDTDg04biOvhFhyyWvOHQfeF_PxMQ\",\"y\":\"e8lnCO-AlStT-NJVX-crhB7QRYhiix03illJOVAOyck\"}";
  jwk_t * jwk_eph, * jwk_bob;
  jwe_t * jwe;
  char * token;
  unsigned char expected_key[] = {86, 170, 141, 234, 248, 35, 109, 32, 92, 34, 40, 205, 113, 167, 16, 26};
  
  ck_assert_int_eq(r_jwk_init(&jwk_eph), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_bob), RHN_OK);
  ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_eph, eph), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk_bob, bob), RHN_OK);
  ck_assert_int_eq(r_jwe_set_payload(jwe, (const unsigned char *)PAYLOAD, o_strlen(PAYLOAD)), RHN_OK);
  ck_assert_int_eq(r_jwe_add_keys(jwe, jwk_eph, jwk_bob), RHN_OK);

  ck_assert_int_eq(r_jwe_set_alg(jwe, R_JWA_ALG_ECDH_ES), RHN_OK);
  ck_assert_int_eq(r_jwe_set_enc(jwe, R_JWA_ENC_A128GCM), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apu", "QWxpY2U"), RHN_OK);
  ck_assert_int_eq(r_jwe_set_header_str_value(jwe, "apv", "Qm9i"), RHN_OK);
  
  ck_assert_ptr_ne((token = r_jwe_serialize(jwe, NULL, 0)), NULL);
  o_free(token);
  
  ck_assert_int_eq(sizeof(expected_key), jwe->key_len);
  ck_assert_int_eq(0, memcmp(jwe->key, expected_key, jwe->key_len));
  
  r_jwk_free(jwk_eph);
  r_jwk_free(jwk_bob);
  r_jwe_free(jwe);
}
END_TEST

#endif

static Suite *rhonabwy_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("Rhonabwy JWE ECDH-ES encryption tests");
  tc_core = tcase_create("test_rhonabwy_ecdh_es");
#if NETTLE_VERSION_NUMBER >= 0x030600
  tcase_add_test(tc_core, test_rhonabwy_encrypt_decrypt_ok);
  tcase_add_test(tc_core, test_rhonabwy_encrypt_decrypt_x25519_ok);
  tcase_add_test(tc_core, test_rhonabwy_encrypt_decrypt_x448_ok);
  tcase_add_test(tc_core, test_rhonabwy_encrypt_invalid_parameters);
  tcase_add_test(tc_core, test_rhonabwy_decrypt_invalid_key);
  tcase_add_test(tc_core, test_rhonabwy_decrypt_invalid_x25519_key);
  tcase_add_test(tc_core, test_rhonabwy_decrypt_invalid_x448_key);
  tcase_add_test(tc_core, test_rhonabwy_encrypt_invalid_key_type);
  tcase_add_test(tc_core, test_rhonabwy_parse_token_invalid);
  tcase_add_test(tc_core, test_rhonabwy_decrypt_token_invalid);
  tcase_add_test(tc_core, test_rhonabwy_flood_ok);
  tcase_add_test(tc_core, test_rhonabwy_check_key_length_ecdh_es);
  tcase_add_test(tc_core, test_rhonabwy_check_key_length_ecdh_es_a256kw);
  tcase_add_test(tc_core, test_rhonabwy_check_key_type_ecdh_es);
  tcase_add_test(tc_core, test_rhonabwy_check_key_type_ecdh_es_a256kw);
  tcase_add_test(tc_core, test_rhonabwy_check_key_length_ecdh_es_ecdsa);
  tcase_add_test(tc_core, test_rhonabwy_check_key_length_invalid_ecddsa_key);
  tcase_add_test(tc_core, test_rhonabwy_check_key_length_invalid_eddsa_key);
  tcase_add_test(tc_core, test_rhonabwy_check_apu);
  tcase_add_test(tc_core, test_rhonabwy_check_apv);
  tcase_add_test(tc_core, test_rhonabwy_rfc_ok);
#endif
  tcase_set_timeout(tc_core, 30);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(int argc, char *argv[])
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  //y_init_logs("Rhonabwy", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting Rhonabwy JWE ECDH-ES encryption tests");
  r_global_init();
  s = rhonabwy_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  r_global_close();
  //y_close_logs();
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
