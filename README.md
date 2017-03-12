H3LEnhanced
======================

[![Hu3-Life Teaser Trailer 3](https://img.youtube.com/vi/fvkc70QQid0/0.jpg)](https://www.youtube.com/watch?v=fvkc70QQid0)

Olá! Sejam bem-vindos à zona!

O nosso objetivo é criar a ultimate experiência 100% cagada 3D vulgarmente conhecida como "**Hu3-Life**".

Tudo isso será possível graças a esse pequeno milagre chamado **HLEnchanced**, que se trata de um remake espetacurlamente bem feito do HL1. (Violá-lo-emos!)

Nesse primeiro momento o que nos importa é trazer para cá todas as nossas hu3lterações implementadas no [velho HL1 SDK](https://github.com/ValveSoftware/halflife) da Valve, processo que deve levar um tempo - não fiquem ansiosos.

Mas mesmo que os pedaços do Hu3 ainda estejam faltando por aqui, já estamos prontos para testes! Vocês podem nos mandar sugestões, códigos, apontar erros, etc. Sintam-se à vontade para destruir e obliterar!

Nossa única ressalva é que não nos comprometemos a resolver bugs no HL1 nem no HLEnchanced. [Eis o local](https://github.com/SamVanheer/HLEnhanced/issues) para que vocês falem com os *super programadores* competentes dessa causa. Notem que é até melhor que os seus remendos sejam feitos lá já que os forks se beneficiam dos avanços.

Testes
----------------

Estamos utilizando o próprio mini mod oficial do HLEnchanced para checar as mudanças, fiquem com o download:

[https://www.dropbox.com/s/fe66rijzdqexk4q/hlenhanced.zip?dl=0](https://www.dropbox.com/s/fe66rijzdqexk4q/hlenhanced.zip?dl=0) (~27MB)

Essas são as nossas modificações:
* incluímos os nossos client.dll e o hl.dll modificados;
* incluímos o sample.wad;
* incluímos alguns dos nossos arquivos de configuração;
* incluímos os mapas hl_c00, hl_c01_a1 e hl_c01_a2 do Sven Coop para realizar testes no modo coop;
* incluímos alguns sprites;
* o nome da pasta do mod já está acertado; e
* alguns arquivos já estão corretamente renomeados.

Para instalar, descompactem o zip dentro da pasta ".../steamapps/common/Half-Life" e reiniciem o Steam.

Obs: o download está hospedado no Dropbox e sem avisos nós mexemos nele adicionando ou removendo coisas. Não há controle de versão pois essa coletânea não passa de um conjunto de testes. Além disso, entendam que, infelizmente, vocês pouco vão encontrar do Hu3-Life por aqui. Esse repositório possui o motor do mod, ele não é o mod em si.

Compilação
----------------

Nós seguimos e recomenamos as instruções do repositório original. Compilamos o projeto tanto no Windows 7 com o [Visual Studio 2015 Community Edition](https://www.visualstudio.com/downloads/)) quanto no Linux Mint 18.1 com gcc-5 e g++-5, tudo focado em 32 bits. Foi necessário baixar o CMake atualizado nos dois casos.

Acompanhem por lá: [https://github.com/SamVanheer/HLEnhanced/wiki](https://github.com/SamVanheer/HLEnhanced/wiki)

Lista de mudanças
----------------

Começará a ser escrita em breve... Provavelmente será linkada aqui.

Obrigado
----------------

Façam bom uso, e agradecemos adiantado aos que nos ajudarem na zoeira. Essa pessoas têm os nomes garantidos nos créditos do mod.

Half Life 1 SDK LICENSE
======================

Half Life 1 SDK Copyright© Valve Corp.  

THIS DOCUMENT DESCRIBES A CONTRACT BETWEEN YOU AND VALVE CORPORATION (“Valve”).  PLEASE READ IT BEFORE DOWNLOADING OR USING THE HALF LIFE 1 SDK (“SDK”). BY DOWNLOADING AND/OR USING THE SOURCE ENGINE SDK YOU ACCEPT THIS LICENSE. IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE PLEASE DON’T DOWNLOAD OR USE THE SDK.

You may, free of charge, download and use the SDK to develop a modified Valve game running on the Half-Life engine.  You may distribute your modified Valve game in source and object code form, but only for free. Terms of use for Valve games are found in the Steam Subscriber Agreement located here: http://store.steampowered.com/subscriber_agreement/ 

You may copy, modify, and distribute the SDK and any modifications you make to the SDK in source and object code form, but only for free.  Any distribution of this SDK must include this license.txt and third_party_licenses.txt.  
 
Any distribution of the SDK or a substantial portion of the SDK must include the above copyright notice and the following: 

DISCLAIMER OF WARRANTIES.  THE SOURCE SDK AND ANY OTHER MATERIAL DOWNLOADED BY LICENSEE IS PROVIDED “AS IS”.  VALVE AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES WITH RESPECT TO THE SDK, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE AND FITNESS FOR A PARTICULAR PURPOSE.  

LIMITATION OF LIABILITY.  IN NO EVENT SHALL VALVE OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THE ENGINE AND/OR THE SDK, EVEN IF VALVE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  
 
 
If you would like to use the SDK for a commercial purpose, please contact Valve at sourceengine@valvesoftware.com.
