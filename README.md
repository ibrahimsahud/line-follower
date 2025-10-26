# Line Follower Robot (Arduino Mega 2560)

وصف المشروع:
هذا مشروع روبوت متتبع خط مكتوب لـ Arduino Mega 2560، يستخدم 5 حساس IR، محركان مدعومان بواسطة لوحة قيادة IBT-2، بالإضافة إلى قارئ RFID وLIDAR للمسافة. الكود شامل ووارد في `Line+Follower.ino`.

معلومات المنافذ (Pins) المستخدمة في الكود:

- POWER_PIN: 28  -> خرج لتزويد/تمكين دائرة/حساسات (مُستخدم كمفتاح طاقة).
- الحساسات (IR sensors):
  - SENSOR1 -> 22
  - SENSOR2 -> 24
  - SENSOR3 -> 25
  - SENSOR4 -> 26
  - SENSOR5 -> 27

- محركات (IBT-2):
  - LEFT_RPWM  -> 5
  - LEFT_LPWM  -> 6
  - LEFT_R_EN  -> 7
  - LEFT_L_EN  -> 8
  - RIGHT_RPWM -> 9
  - RIGHT_LPWM -> 10
  - RIGHT_R_EN -> 11
  - RIGHT_L_EN -> 12

- RFID & Buzzer:
  - RST_PIN (RFID reset) -> 2
  - SDA_PIN (RFID SDA/SS) -> 53 (SPI SS)
  - BUZZER_PIN -> 4

- LIDAR:
  - متصل إلى Serial1 بسرعة 115200 (استخدم Serial1 على Arduino Mega).
  - قيمة العتبة في الكود: `threshold = 50` سم (إذا كانت المسافة أقل أو تساوي هذه القيمة، يتوقف الروبوت).

إعدادات السرعات في الكود:
- NORMAL_SPEED = 100
- CORRECTION_SPEED = 20
- TURN_SPEED = 130
- SEARCH_SPEED = 80

ملاحظات توصيل/تحضيرية:
- الكود موجه لـ Arduino Mega 2560. تأكد من اختيار اللوحة الصحيحة في Arduino IDE.
- قارئ MFRC522 يعمل على واجهة SPI: تأكد من توصيل MOSI/MISO/SCK وSDA (pin 53 هنا) وRST (pin 2). VCC يجب أن يكون 3.3V.
- LIDAR متصل بـ Serial1 (RX/TX الخاصة بـ Serial1 على Mega عادةً: TX1=18, RX1=19) — تحقق من وصلاتك وفقاً لطراز LIDAR المستخدم.
- محركات: IBT-2 يحتاج إلى مصدر طاقة للمحركات منفصل (مثلاً بطارية 7.4V أو حسب المحركات)، وتأكد من توصيل GND مشترك مع لوحة الأردوينو.
- POWER_PIN (28) مُستخدم في الكود لتشغيل/تمكين دائرة؛ عدل حسب هيكلية مصدر الطاقة الخاص بك إن لزم.

