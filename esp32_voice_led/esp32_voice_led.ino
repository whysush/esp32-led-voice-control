#include <Arduino.h>
#include <driver/i2s.h>

// ===== EDGE IMPULSE =====
#include <esp32-led-voice-control_inferencing.h>

// ===== INMP441 PINS =====
#define I2S_WS   15
#define I2S_SD   32
#define I2S_SCK  14

// ===== LED =====
#define LED_PIN 25

// ===== AUDIO SETTINGS =====
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 16000
#define BUFFER_LEN 512
#define AUDIO_BUFFER_SIZE EI_CLASSIFIER_RAW_SAMPLE_COUNT

int16_t i2s_samples[BUFFER_LEN];
int16_t audio_buffer[AUDIO_BUFFER_SIZE];
int audio_index = 0;

// ===== I2S SETUP =====
void setup_i2s() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

// ===== EDGE IMPULSE SIGNAL WRAPPER =====
int get_signal_data(size_t offset, size_t length, float *out_ptr) {
  for (size_t i = 0; i < length; i++) {
    out_ptr[i] = audio_buffer[offset + i] / 32768.0f;
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  setup_i2s();

  Serial.println("ESP32 Voice Control Ready");
}

void loop() {
  size_t bytes_read;
  i2s_read(I2S_PORT, i2s_samples, sizeof(i2s_samples), &bytes_read, portMAX_DELAY);

  int samples_read = bytes_read / 2;

  for (int i = 0; i < samples_read; i++) {
    audio_buffer[audio_index++] = i2s_samples[i];

    if (audio_index >= AUDIO_BUFFER_SIZE) {
      audio_index = 0;

      signal_t signal;
      signal.total_length = AUDIO_BUFFER_SIZE;
      signal.get_data = &get_signal_data;

      ei_impulse_result_t result;
      EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);

      if (err != EI_IMPULSE_OK) {
        Serial.println("Classifier error");
        return;
      }

      float on_score = 0;
      float off_score = 0;

      for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.print(result.classification[i].label);
        Serial.print(": ");
        Serial.println(result.classification[i].value);

        if (strcmp(result.classification[i].label, "on") == 0)
          on_score = result.classification[i].value;

        if (strcmp(result.classification[i].label, "off") == 0)
          off_score = result.classification[i].value;
      }

      Serial.println("----");

      // ===== DECISION LOGIC =====
      if (on_score > 0.7) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      }

      if (off_score > 0.7) {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      }
    }
  }
}
