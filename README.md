# NemoKeyMapper (English)

NemoKeyMapper is a PC program that allows users to change keyboard key mappings and register them in the registry. Since registration in the registry is required, administrative privileges are necessary. There should be no issues if you apply the changes as an administrator when prompted.
The program uses only the Scancode Map data located in `HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Keyboard Layout`. Typically, there is little to no information registered here.
By modifying and registering data here, you can input the mapping information for the keys.
If there is existing mapping data, it's a good idea to back it up in case.
To initialize (remove all mappings), you can insert the following data to return to a state with no mapping information:

Scancode Map (Hexadecimal data)
00 00 00 00 00 00 00 00
01 00 00 00 00 00 00 00

- Supported languages: English, Korean, German, Russian, Spanish, Arabic, Italian, Japanese, Chinese, Portuguese, French, Hindi
- Supported OS: Windows 10, 11 ~

### Installation

Execute `nemokeymapper.exe`

## How to Use
Running the `nemokeymapper.exe` file will launch the program.
You can add to the list by selecting the input key and the mapping key, then pressing the add/change button. After registering the necessary keys, clicking the register button will save your settings.
Clicking the direct input button allows you to receive button inputs to set the input key and mapping key. Clicking deselect will cancel the selected entry from the list, enabling you to input a new key.
Pressing the delete button after selecting from the list will remove the entry.

## Supported Languages

UI: Korean, English, German, Spanish, French, Italian, Japanese, Portuguese, Russian, Chinese, Arabic, Hindi

## Caution

Mapping important keys can cause problems with computer use because the input key may become unavailable for its original function. It is recommended not to map crucial keys.
Media keys may not map correctly even if registered. The current version of the program does not support mapping these keys due to the complexity of implementing low-level HID support, which could take considerable time to develop.

## Terms of Use

The use of the source for the following purposes is prohibited.

* Any act that violates the MIT License and the laws of your country. 

## Disclaimer

I am not liable for any direct, indirect, consequential, incidental, or special damages arising out of or in any way connected with the use/misuse or inability to use this software.

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/realtrans)

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# NemoKeyMapper ( 한국어 )

키보드 키매핑을 변경하여 레지스트에 등록하여 사용할 수 있는 PC용 프로그램입니다. 레지스트에 등록해야하기때문에 관리자 권한이 필요합니다. 실행시에 관리자 변경 확인에서 적용하면 문제가 없습니다.
사용하는 레지스트는 HKEY_LOCAL_MACHINE에 System\CurrentControlSet\Control\Keyboard Layout에 위치한 Scancode Map 데이터만 사용합니다. 보통은 여기에 등록된 정보가 대부분 없습니다.
여기 데이터를 변경하여 등록하는 키의 매핑정보를 넣게됩니다.
혹시라도 기존에 매핑한 데이터가 존재할 경우에 백업을 해두시는 것도 좋은 방법입니다.
초기화 방법은 아래처럼 데이터를 넣으시면 매핑 정보가 없는 상태가 됩니다.

Scancode Map ( Hexa data )
---------------------------
00 00 00 00 00 00 00 00
01 00 00 00 00 00 00 00
---------------------------

- 지원 언어 : 영어, 한국어, 독일어, 러시아어, 스페인어, 아랍어,이탈리아어, 일본어, 중국어, 포르투칼어, 프랑스어, 힌디어
- 지원 OS : Windows 10, 11 ~

### 설치

nemokeymapper.exe 실행

## 사용법
nemokeymapper.exe 파일을 실행하면 프로그램이 실행됩니다.
입력키와 매핑키를 선택해서 추가/변경키를 누르게되면 목록에 추가가 됩니다. 필요한 키를 등록하신후에 레지스트 등록버튼을 클릭하게되면 저장이 완료됩니다.
직접입력 버튼을 클릭하시면 버튼입력을 받아서 입력키와 매핑키를 정하실 수 있습니다. 선택취소를 하시면 목록에서 선택된 것이 취소가 되어 새로운 키를 입력할 수 있습니다.
목록에서 선택후 삭제버튼을 누르시면 삭제가됩니다.

## 지원 언어
 
UI : 한국어, 영어, 독일어, 스페인어, 프랑스어, 이탈리아어, 일본어, 포르투갈어, 러시아어, 중국어, 아랍어, 힌디어

## 주의

중요한 키를 등록하신 경우에는 해당 키가 다른키로 사용되기때문에 입력키가 없는상태가됩니다. 이때문에 컴퓨터 사용에 문제가 될 수 있으니 중요한 키는 매핑하지 않으시는 것을 추천드립니다.
미디어 키는 매핑 등록을 해도 매핑이 되지 않는 현상이 있습니다. 현재의 프로그램으로는 구현하기가 어렵습니다. HID쪽 저수준 구현을 알아보았으나 시간이 많이 걸릴 것 같아서 일단은 이번 버젼에는 구현하지 않았습니다.

## 이용 약관

다음과 같은 목적으로 소스를 사용하는 것을 금지합니다.

* MIT License와 해당 국가의 법률에 위배되는 행위

## 부연 설명

이 소프트웨어의 사용으로 발생하는 직접적, 간접적, 결과적, 우발적 또는 특별한 손해에 대해 책임을 지지 않습니다.

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/realtrans)

