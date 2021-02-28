const int pot_out[] = { 1, 2, 3, 4, 5};
const int butt_out[] = { 27, 29, 31, 33 };
const int sw_butt_out[] = { 23, 25 };
const int sw_val[] = { 127, 64 };
const int led_out[] = { 9, 10, 11, 12 };
const int sw_led_out[] = { 7, 8 };

const int POT_NUM = sizeof pot_out / sizeof (int);
const int BUTT_NUM = sizeof butt_out / sizeof (int);
const int SW_BUTT_NUM = sizeof sw_butt_out / sizeof (int);
const int LED_NUM = sizeof led_out / sizeof (int);
const int SW_LED_NUM = sizeof sw_led_out / sizeof (int);

const int PORT_RATE = 31250; // 31250 / 9600
const int MIDI_CHANNEL = 12;
const int MIN_CH = 5;
const int POT_SHIFT =7;
const int BUTT_SHIFT = POT_NUM + POT_SHIFT;
const int SW_BUTT_SHIFT = BUTT_SHIFT + BUTT_NUM;

int pots[POT_NUM] = { 0 };
int buttons[BUTT_NUM];
int curr_val;

inline void ctrl_setup(void (*func)(uint8_t, uint8_t), const int *arr, const int size, const int val) {
  for (int i = 0; i < size; ++i)
    func(arr[i], val);
}

inline void midi_send(int cc, int val) {
  Serial.write(176 | (MIDI_CHANNEL & 15));
  Serial.write(cc);
  Serial.write(val);
}

void setup() {
	for (int button = 0; button < BUTT_NUM; ++button)
		buttons[button] = 127;

  ctrl_setup(pinMode, pot_out, POT_NUM, INPUT);
  ctrl_setup(pinMode, butt_out, BUTT_NUM, INPUT);
  ctrl_setup(pinMode, sw_butt_out, SW_BUTT_NUM, INPUT);
  ctrl_setup(pinMode, led_out, LED_NUM, OUTPUT);
  ctrl_setup(pinMode, sw_led_out, SW_LED_NUM, OUTPUT);
  ctrl_setup(digitalWrite, led_out, LED_NUM, LOW);
  ctrl_setup(digitalWrite, sw_led_out, LED_NUM, LOW);

  midi_send(SW_BUTT_SHIFT, sw_val[0]);
  digitalWrite(sw_led_out[0], HIGH);

  Serial.begin(PORT_RATE);
}

void loop() {
  for (int pot = 0; pot < POT_NUM; ++pot) {
		curr_val = analogRead(pot_out[pot]);
		if (abs(pots[pot] - curr_val) > MIN_CH) {
			pots[pot] = curr_val;
			midi_send(pot + POT_SHIFT, curr_val / 8); 
		}
  }

  for (int button = 0; button < BUTT_NUM; ++button) {
    curr_val = digitalRead(butt_out[button]);
    if (curr_val == HIGH) {
      digitalWrite(led_out[button], buttons[button] & 1);
      midi_send(button + BUTT_SHIFT, (buttons[button] ^= 127));
      delay(500);
    }
  }

  for (int sw_button = 0; sw_button < SW_BUTT_NUM; ++sw_button) {
    curr_val = digitalRead(sw_butt_out[sw_button]);
    if (curr_val == HIGH) {
      digitalWrite(sw_led_out[sw_button], HIGH);
      midi_send(SW_BUTT_SHIFT, sw_val[sw_button]);
      for (int i = 0; i < SW_BUTT_NUM; ++i) {
        if (i != sw_button)
          digitalWrite(sw_led_out[i], LOW);
      }
      delay(500);
    }
  }
}
