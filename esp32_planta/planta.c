// Проект "Цветок" для конструктора кода ESP32
// Настройки: 
// [0]:Включение полива, 
// [1]:Кол-во у.е для помпы, 
// [2]:Задержка при путом баке,
// [3]:Время вкл.часы, 
// [4]:Время вкл. мин., 
// [5]:Цветок 1, 
// [6]:Цветок 2,
// [7]:Цветок 3, 
// [8]:Цветок 4, 
// [9]:Цветок 5,
// [10]:Цветок 6, 
// [11]:Цветок 7, 
// [12]:Цветок 8
// [13]:


// Глобальные переменные: 7 шт.
// valdes[0] - Вода в баке
// valdes[1] - Освещенность
// valdes[2] - Температура
// valdes[3] - Счетчик воды
// valdes[4] - Влага 1
// valdes[5] - Доза     1
// valdes[6] - принудительное включение подсветки по времени

void startfunc() {  // выполняется один раз при старте модуля.
  //sensors_param.cfgdes[12]=sensors_param.cfgdes[12]+1;SAVEOPT
    valdes[2] = 0; // разрешение работать подсветке по времени
    valdes[4] = 0; // минут после последнего полива
    valdes[5] = 0; // переменная нажатой кнопки

    valdes[3] = 0;   // задание для подсветки
    valdes[0] = 0;   // текущая яркость позсветки
    valdes[1] = 0;   // яркость подсветки в процентах
    valdes[6] = 0;   // принудительное включение подсветки по времени
    valdes[0] = sensors_param.cfgdes[0];

    digitalWrite(5, 1);
    adc1_config_channel_atten(0, 2);
}

void setPoliv(int doz)
{
    if(sensors_param.cfgdes[0]==0) return;
    valdes[1]=0;
    countint[0] = 0;
    digitalWrite(5, 0);
    digitalWrite(25, 1);
    int step = 0;
      float x_cnt=100;
      //x_cnt=100/Settings.cntVoda;
      float y_cnt=sensors_param.cfgdes[1]*10;  //*10       //170
      float z_cnt=doz;                     //219
      float r_cnt=(z_cnt/x_cnt*y_cnt);    //   219/(100/170)
      int x_doza=(int)r_cnt;  // 100=36 100/36
      //cntVoda<x_doza
    while (countint[0] < x_doza)
    {
        delay(10);
        step++;
        if (step > 500) { valdes[1]=1; break; }
    }
    digitalWrite(25, 0);
    digitalWrite(5, 1);
}
int getDoza(int typ, int size, int vlaga)
{
    float s = size / 10;
    return 1;
}   

void setAdr(int adr)
{
    switch (adr)
    {
    case 0: digitalWrite(3, 0); digitalWrite(21, 0); digitalWrite(19, 0); digitalWrite(18, 0); break;
    case 1: digitalWrite(3, 1); digitalWrite(21, 0); digitalWrite(19, 0); digitalWrite(18, 0); break;
    case 2: digitalWrite(3, 0); digitalWrite(21, 1); digitalWrite(19, 0); digitalWrite(18, 0); break;
    case 3: digitalWrite(3, 1); digitalWrite(21, 1); digitalWrite(19, 0); digitalWrite(18, 0); break;
    case 4: digitalWrite(3, 0); digitalWrite(21, 0); digitalWrite(19, 1); digitalWrite(18, 0); break;
    case 5: digitalWrite(3, 1); digitalWrite(21, 0); digitalWrite(19, 1); digitalWrite(18, 0); break;
    case 6: digitalWrite(3, 0); digitalWrite(21, 1); digitalWrite(19, 1); digitalWrite(18, 0); break;
    case 7: digitalWrite(3, 1); digitalWrite(21, 1); digitalWrite(19, 1); digitalWrite(18, 0); break;
    case 8: digitalWrite(3, 0); digitalWrite(21, 0); digitalWrite(19, 0); digitalWrite(18, 1); break;
    default:
        digitalWrite(3, 0); digitalWrite(21, 0); digitalWrite(19, 0); digitalWrite(18, 0);
        break;
    }
}
int getVlaga()
{
    //digitalWrite(13, 1);
    digitalWrite(14, 0);
    delay(100); 
    int vlaga = adc1_get_raw(3);
    digitalWrite(14, 1);
    //digitalWrite(13, 0);
    return vlaga;
}

void testVlaga()
{
    digitalWrite(13, 1);
    delay(1000);
    for (int i = 0; i < 8; i++)
    {
        int typ = sensors_param.cfgdes[5 + i] / 10000000;
        if (typ == 6) continue;
        
        setAdr(i);
        delay(500);
        int vlaga = getVlaga();
        valdes[4 + i] = vlaga;
        int doza = CalcDoza(i);
        //if (doza == 0) {
        //    doza = getDoza(typ, (sensors_param.cfgdes[5] % 1000000 / 10000), vlaga);
        //}
        valdes[12 + i] = doza;
    }
    digitalWrite(13, 0);  
}

void poliv()
{
    int doza = 1;
    //valdes[0] = 50;
    digitalWrite(13, 1);
    delay(1000);
    for (int i = 0; i < 8; i++)
    {
        int typ = sensors_param.cfgdes[5 + i] / 10000000;
        if (typ == 6) continue;
        doza = sensors_param.cfgdes[5 + i] % 1000;
        setAdr(i);
        delay(500);
        int vlaga = getVlaga();
        valdes[4 + i] = vlaga;
        doza = CalcDoza(i);
        //if (doza == 0) {
        //    doza = getDoza(typ, (sensors_param.cfgdes[5] % 1000000 / 10000), vlaga);
        //}
        valdes[12 + i] = doza;
        setPoliv(doza);
    }
    digitalWrite(13, 0);
}


void timerfunc(uint32_t  timersrc) {// раз 1 секунду
    if (valdes[3]) {                    // обработка нажатия кнопок на web-странице
        int32_t buf = valdes[3] / 100000000;
        switch (buf) {
        case 1:
            sensors_param.cfgdes[6] = valdes[3] % 100000000; SAVEOPT
                break;
        case 2:
            sensors_param.cfgdes[0] = valdes[3] % 100000000; 
	        valdes[0] = sensors_param.cfgdes[0];
                SAVEOPT
                break;
        case 3:
            sensors_param.cfgdes[4] = valdes[3] % 100000000; SAVEOPT
                break;
        case 4:
            sensors_param.cfgdes[3] = (valdes[3] % 100000000) / 10000000;
            sensors_param.cfgdes[4] = (valdes[3] % 10000000) / 100000;
            //sensors_param.cfgdes[5] = (valdes[3] % 10000) ;
            //sensors_param.cfgdes[5] = 
            //char data[10];
            //    os_sprintf(data, "%1d%1d%1d%1d", (valdes[3]%10000)/1000, (valdes[3] % 1000) / 100, (valdes[3] % 100) / 10, (valdes[3] % 10) );
            //    sensors_param.cfgdes[5] = data;
            SAVEOPT
                break;
        case 7:

            sensors_param.cfgdes[4 + valdes[3] % 10000000 / 1000000] = valdes[3] % 100000000;
            //sensors_param.cfgdes[5] = 
            //char data[10];
            //    os_sprintf(data, "%1d%1d%1d%1d", (valdes[3]%10000)/1000, (valdes[3] % 1000) / 100, (valdes[3] % 100) / 10, (valdes[3] % 10) );
            //    sensors_param.cfgdes[5] = data;
            SAVEOPT
                break;
        case 5:
            poliv();
            break;
        case 6:
            //valdes[6]=valdes[3]%100000000;
            break;
        }
        valdes[3] = 0;
    }
    if (timersrc % 60 == 10) {   // раз в минуту
        static int32_t sum_en = 0;
        valdes[2]++;
        if(valdes[2]==sensors_param.cfgdes[13])
        {
            valdes[2]=0;
            testVlaga();
        }
        if(sensors_param.cfgdes[0]==1)
        {
            if(sensors_param.cfgdes[3]==time_loc.hour && sensors_param.cfgdes[4]==time_loc.min)
            {
                poliv();
            }
        }
        //if(!sensors_param.cfgdes[5]){valdes[4]=sensors_param.cfgdes[12];}
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}



float Get_Vlaga(unsigned int vl)
{
    //  200
    //150 - 10%
    //vl - x%  //(12+(30-12)/(830-400)*((900-vl)-400))
    float l_vl=vl/4;
    if (l_vl > 1000) l_vl = 1000.0;
    /*
    float v_1 = 1000.0 - l_vl;
    v_1 = v_1 - 400.0;
    float v_2 = 0.042; //18.0 / 430.0;
    //v_2=v_2/(int)430;
    v_2 = v_2 * v_1;
    v_1 = 12.0 + v_2;
    //int rr=(int)(12+18/430*v_1);
    */
    float v_1=12.01+(0.0286)*((1050.0-vl)-200.0);  //12.01+(30-12)/(830-400)*((1050-vl)-200)
    return v_1;
}

signed int CalcDoza(int chanel)
{
    //   {Гигроскопическая влажность (ГВ),	
    //    Влажность завядания растений (ВЗР) = ГВ*1,5	
    //    Предельная полевая влагоемкость (ППВ)	
    //    Плотность почвы, кг / дм куб (/10)}
    unsigned int zemlya[4][4] = { {15, 23, 40, 120}, {6, 9, 22, 120}, {10, 15, 25, 120}, {8, 12, 30, 120} };
    //Суккулент	Песчаный субстрат
    //Плодовые	Торфяной субстрат
    //Цветущие	Торфяной субстрат
    //Орхидея		Орхидейный субстрат
    //Не известно	Обычная земля
    //   {тип земли, Нижний предел влажности, Верхний предел влажности, Коэф летних месяцев(/10), Коэф зимних месяцев(/10)}
    unsigned int flowerList[5][5] = { {1,50,70,10,8}, {0,70,80,10,9}, {0,70,80,10,9}, {2,30,80,10,9}, {3,60,80,10,9} };

    unsigned int l_type = sensors_param.cfgdes[5 + chanel] / 10000000;
    unsigned int l_size = (sensors_param.cfgdes[5 + chanel] % 1000000 / 10000);
    unsigned int l_doza = sensors_param.cfgdes[5 + chanel] % 1000;
    //l_type--;
    float l_V = 0;
    if (l_type == 4) {
        l_V = l_doza;
        //((Settings.channel[chanel].sizeFl*100)/7)+(Settings.channel[chanel].doseFl);	//7 = 146ml na 1L   //5 = 200 ml nf 1L
        if (l_V < 0) l_V = 0;
        return (unsigned int)l_V;
    }
    else {
        //==================
        float l_E = l_size; //размер горшка
        unsigned int l_F = l_type; //тип цветка
        unsigned int l_G = flowerList[l_F][0]; //тип почвы
        float l_H = zemlya[l_G][3]; //плотность
        float l_I = zemlya[l_G][1]; //завядание
        float l_J = zemlya[l_G][2]; //завядание
        //float _M = sys_month; //
        float l_K = (flowerList[l_F][1]); //*(GetKoef(_M, _F)/10); // *коеф
        float l_L = (flowerList[l_F][2]); //*(GetKoef(_M, _F)/10); // *коеф
        float l_N = Get_Vlaga((valdes[4 + chanel]));
        float l_O = (l_J * l_K) / 100;  //%
        float l_P = (l_J * l_L) / 100;  //%
        float l_Q = (l_O + l_P) / 2;
        //((15-12)*((120/100)*1000*(30/10)))/100=
        //=(P6-N6)*H6*1000*E6
        float l_R = ((l_P - l_N) * (l_H / 100)) / 100 * 1000 * (l_E / 10);
        //((($pJ*$pL/100)-floatval($data->list->c1_h[$ii]->vl))*$pH/100*(floatval($data->list->c1->s)/10)*1000)
        //float l_R= (((l_P)-(GetVlaga(l_N)))*l_H/100*((l_E/10))*1000);
        
        int l_S = 0;
        //=IF(N6<=O6,2,(IF(N6>=Q6,1,0)))
        
        if (l_N <= l_O) { l_S = 2; }
        else {
            if (l_N >= l_Q) { l_S = 1; }
            else {l_S = 0;
            }
        }
        //SetError(chanel, _S);
        float l_T = 1;
        //=IF(S6=0,0,IF(S6=1,-1,0.1))
        if (l_S == 0) l_T = 0;
        else if (l_S == 1) l_T = -1;
        else l_T = 1;
        unsigned int l_U = 0; // GetError(chanel);
        //=R6+R6*T6*U6
        l_V = l_R/10; // +l_R * ((l_T)*l_U);
        //if(_V<0) _V=_V*-1;	      
        return (signed int)l_V;
    }
}


void webfunc(char* pbuf) { // вывод данных на главной модуля
    os_sprintf(HTTPBUFF, "Освещение: %d (0-4095)<br>", adc1_get_raw(0));
    os_sprintf(HTTPBUFF, "Температура: %d С<br>", dht_t1); //dht_t1); //_DSW0_
    os_sprintf(HTTPBUFF, "Влажность: %d %%<br>", dht_h1); //dht_h1);
    //data1wire[x]	Температура датчиков DS18B20

    //if(!digitalRead(22)){os_sprintf(HTTPBUFF,"<b><font color='red'>Вода в баке <blink>закончилась!</blink></font></b><br>");}

    os_sprintf(HTTPBUFF, "<hr>");
    os_sprintf(HTTPBUFF, "<b>Контроль функций:</b><br>");
    os_sprintf(HTTPBUFF, "<button type='button' onclick='func(5, 0);repage()' style='width:195px;height:20px'><b>Полить один раз</b></button>");
    if (sensors_param.cfgdes[0]) {
        os_sprintf(HTTPBUFF, "<button type='button' onclick='func(2, 0);repage()' style='width:130px;height:20px;color:#FFF;background:#00FF00'><b>Полив авто</b></button>");
    }
    else {
        os_sprintf(HTTPBUFF, "<button type='button' onclick='func(2, 1);repage()' style='width:130px;height:20px'><b>Полив выкл</b></button>");
    }


    os_sprintf(HTTPBUFF, "<div class='c'><div class='main'><pre><table name='table1'border='0'class='catalogue'><b>Время выполнения полива:</b><tr style='background-color: yellow'><td> часы </td><td> минуты </td></tr>");
    os_sprintf(HTTPBUFF, "<tr><td> %02d </td><td> %02d</td></tr></table>", sensors_param.cfgdes[3], sensors_param.cfgdes[4]);

    os_sprintf(HTTPBUFF, "<table name='table2' border='0' class='catalogue'><b>Настройка цветков:</b><tr style='background-color: yellow'><td> № </td><td> Тип </td><td> Размер </td><td> Доза </td></tr>");

    os_sprintf(HTTPBUFF, "<tr><td> 1 </td><td> %1d</td><td> %2d</td><td> %3d</td>", sensors_param.cfgdes[5] / 10000000, (sensors_param.cfgdes[5] % 1000000 / 10000), sensors_param.cfgdes[5] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");
    os_sprintf(HTTPBUFF, "<tr><td> 2 </td><td> %1d</td><td> %2d</td><td> %3d</td>", sensors_param.cfgdes[6] / 10000000, sensors_param.cfgdes[6] % 1000000 / 10000, sensors_param.cfgdes[6] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");

    os_sprintf(HTTPBUFF, "<tr><td> 3 </td><td> %1d</td><td> %2d</td><td> %3d</td>", sensors_param.cfgdes[7] / 10000000, sensors_param.cfgdes[7] % 1000000 / 10000, sensors_param.cfgdes[7] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");
    os_sprintf(HTTPBUFF, "<tr><td> 4 </td><td> %1d</td><td> %2d</td><td>%3d</td>", sensors_param.cfgdes[8] / 10000000, sensors_param.cfgdes[8] % 1000000 / 10000, sensors_param.cfgdes[8] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");
    os_sprintf(HTTPBUFF, "<tr><td> 5 </td><td> %1d</td><td> %2d</td><td> %3d</td>", sensors_param.cfgdes[9] / 10000000, sensors_param.cfgdes[9] % 1000000 / 10000, sensors_param.cfgdes[9] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");
    os_sprintf(HTTPBUFF, "<tr><td> 6 </td><td> %1d</td><td> %2d</td><td> %3d</td>", sensors_param.cfgdes[10] / 10000000, sensors_param.cfgdes[10] % 1000000 / 10000, sensors_param.cfgdes[10] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");
    os_sprintf(HTTPBUFF, "<tr><td> 7 </td><td> %1d</td><td> %2d</td><td> %3d</td>", sensors_param.cfgdes[11] / 10000000, sensors_param.cfgdes[11] % 1000000 / 10000, sensors_param.cfgdes[11] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");
    os_sprintf(HTTPBUFF, "<tr><td> 8 </td><td> %1d</td><td> %2d</td><td> %3d</td>", sensors_param.cfgdes[12] / 10000000, sensors_param.cfgdes[12] % 1000000 / 10000, sensors_param.cfgdes[12] % 1000);
    os_sprintf(HTTPBUFF, "</tr>");

    os_sprintf(HTTPBUFF, "</table></pre></div>");



    os_sprintf(HTTPBUFF, "<script>var request = new XMLHttpRequest();");
    os_sprintf(HTTPBUFF, "function reqReadyStateChange(){if(request.readyState == 4){var status = request.status;if (status == 200) {document.getElementById('output').innerHTML=request.responseText+ ' wait data save...';}}}");
    os_sprintf(HTTPBUFF, "function func(confset, valset){valset=confset*100000000+valset;request.open('GET', 'valdes?int=3&set='+valset, true);request.onreadystatechange = reqReadyStateChange;request.send();}");
    os_sprintf(HTTPBUFF, "function func2(){var buf = parseInt(cfg8.value)*10000000+parseInt(cfg9.value)*100000+parseInt(cfg1_1.value)*1000+parseInt(cfg1_2.value)*100+parseInt(cfg1_3.value)*10+parseInt(cfg1_4.value);func(4, buf);repage();}");
    os_sprintf(HTTPBUFF, "function repage(){setTimeout('location.reload(true);', 2000);}</script>");

    os_sprintf(HTTPBUFF, "<hr>");
    os_sprintf(HTTPBUFF, "<div id='output'></div>");
}