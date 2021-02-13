// Реализация энкодера через прерывания.
// Фильтрация дребезга контактов аппаратная.


#define GPIO_INPUT_IO_0     21 
#define GPIO_INPUT_IO_1     22 
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT 0

  static int32_t push = 0;
  static int32_t setval = 0;
  static int32_t tichours = 0;
  
// обработчики прерываний обязательно до первого упоминания в коде
static void IRAM_ATTR electro_isr_handler() // обработчик прерывания кнопки
{
  int32_t tmp = digitalRead(GPIO_INPUT_IO_0);
  if(tmp==setval) return;
  else setval=tmp;
  
  valdes[0]=valdes[0]+1; //sensors_param.cfgdes[1]+1;
  tichours=tichours+1;
  valdes[3]=tichours;
  if(valdes[0]==sensors_param.cfgdes[0])
  {
    valdes[0]=0;
    valdes[1]=valdes[1]+1; //sensors_param.cfgdes[2]+1;
  }
  //sensors_param.cfgdes[1]=valdes[0];
  //sensors_param.cfgdes[2]=valdes[1];
  push=1;
  //vTaskDelay( portTICK_PERIOD_MS);
  //SAVEOPT
}

static void IRAM_ATTR gaz_isr_handler() // обработчик прерывания поворотного диска
{
  valdes[2]=valdes[2]+1; //sensors_param.cfgdes[3]+1;
  //sensors_param.cfgdes[3]=valdes[2];
  //SAVEOPT
  push=1;
  //vTaskDelay( portTICK_PERIOD_MS);
  
}

void startfunc(){ // выполняется один раз при старте модуля.
  valdes[0]=sensors_param.cfgdes[1];
  valdes[1]=sensors_param.cfgdes[2];
  valdes[2]=sensors_param.cfgdes[3];
  valdes[3]=0;
  valdes[4]=1;
  push=0;
  tichours=0;
  //sensors_param.cfgdes[0]
  // первое обновление дисплея
  char data[30];

  // настройка режимов GPIO
  gpio_config_t io_conf;
  // отключаем прерывания
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  // срабатывание прерывания по фронту
  io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
  // выбираем GPIO21 и GPIO22
  io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
  // режим выбранных портов на вход
  io_conf.mode = GPIO_MODE_INPUT;
  // включение внутренней подтяжки GPIO к питанию
  io_conf.pull_up_en = 1;
  gpio_config(&io_conf);

  // изменение выбранного GPIO на срабатывание по спаду
  // gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_NEGEDGE);

  // инициализация сервиса прерывания GPIO
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  // добавляем срабатывание прерывания для GPIO21
  gpio_isr_handler_add(GPIO_INPUT_IO_0, electro_isr_handler, NULL);
  // добавляем срабатывание прерывания для GPIO22
   gpio_isr_handler_add(GPIO_INPUT_IO_1, gaz_isr_handler, NULL);
}

void timerfunc(uint32_t  timersrc) {  // выполнение кода каждую 1 секунду
  // локальные нестираемые переменные

  // если нажимали кнопку энкодера, то изменения на дисплей
  if(push==1){
    push=0;
    if(valdes[4]==1) 
    {
      ;//SAVEOPT
    }
  }

  if(timersrc%30==0){ // выполнение кода каждые 30 секунд
    
  }
  if(time_loc.min==0 && time_loc.sec==0)
  {
    //char topic[30];
		//char payload[16];
    ////MQTT_Publish("statistic/gaz/day_begin", tichours, 0, true, true, 0);
    //sprintf(topic,"statistic/gaz/day_begin");
		//sprintf(payload,"%d",tichours);
    //MQTT_Publish(topic, payload, strlen(payload), 2, 0, 0);
    tichours=0;
  } 

  // задержка функции timerfunc()
  vTaskDelay(500 / portTICK_PERIOD_MS); 
}

void webfunc(char *pbuf) { // вывод данных на главной модуля
  // можно увидеть и на вебморде, но не динамично
  //os_sprintf(data,"CntLoop: %d",sensors_param.cfgdes[0]);
  //LCD_print(0,data);
  //os_sprintf(data,"TicEl: %d",sensors_param.cfgdes[1]);
  //LCD_print(1,data);
  //os_sprintf(data,"CntEl: %d",sensors_param.cfgdes[2]);
  //LCD_print(2,data);
  os_sprintf(HTTPBUFF,"</div><div class=blockk>");
  os_sprintf(HTTPBUFF,"CntLoop: %d<br>",sensors_param.cfgdes[0]);
  os_sprintf(HTTPBUFF,"TicEl: %d<br>",valdes[0]);
  os_sprintf(HTTPBUFF,"CntEl: %d<br>",valdes[1]);
  os_sprintf(HTTPBUFF,"CntGaz: %d<br>",valdes[2]);
  os_sprintf(HTTPBUFF,"Init: %d<br>",valdes[4]);
}