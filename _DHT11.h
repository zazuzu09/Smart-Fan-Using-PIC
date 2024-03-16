
#define  Data_Pin RD3                       // ket noi voi chan RD3
#define  Data_Pin_TRIS TRISD3       
//khai bao bien toan cuc
int DATA[5];

int DHT11_start(){
    //----------------------------giao tiep voi DTH11-------------------------
    Data_Pin_TRIS = 0;
    Data_Pin = 0;
    __delay_ms(30); //doi > 18ms
    Data_Pin = 1; 
    Data_Pin_TRIS = 1;
    //----------------------------kiem tra giao tiep DHT11--------------------
    __delay_us(40);                     // doi  20-40us thi data xuong 0
    if(Data_Pin == 0)                          
    {
        __delay_us(80);                 //doi 80us thi Data len 1
        if(Data_Pin == 1)
        {
            __delay_us(80);             
            return 1;    
        }
    }
}

int Read_DTH11(){
    //-----------------nhan du lieu----------------------
    for(int i = 0; i<5; i++){
       for(int ii = 0; ii<8; ii++) {
        while(Data_Pin == 0);                          //doi len 1
        __delay_us(50);                         
        if(Data_Pin == 1)                               //trong >28us ma data van =1 thi se gui bit 1
        {
            DATA[i] =DATA[i]<<1| 1;
            while(Data_Pin == 1){}
        }
        else  DATA[i] =DATA[i]<<1| 0;
       }
    }
    //---------------------kiem tra data nhan duoc-------------------
    if(DATA[4] == ( DATA[3]+ DATA[2]+DATA[1]+ DATA[0])) return 1;
}

void hienthi_DHT11(){
    //------------------in ket qua-----------------------------------------------
    //in nhiet do
    LCD_Clear();    
    LCD_PutString("Nhiet do= ");
    for(int i=13;i>11;i--){
    LCD_Gotoxy(0,i);                                 //hien thi hang don vi truoc >hang chuc  
    LCD_PutChar(DATA[2] %10+48);                      //doi ra so thap phan +48
    DATA[2]=DATA[2]/10;
    }
    //in do am
    LCD_Gotoxy(1,0); 
    LCD_PutString("Do am= ");
    for(int i=13;i>11;i--){
    LCD_Gotoxy(1,i);                                 //hien thi hang don vi truoc >hang chuc  
    LCD_PutChar(DATA[0] %10+48);                      //doi ra so thap phan +48
    DATA[0]=DATA[0]/10;
    }
      //hien thi don vi nhiet do va do am 
    LCD_Gotoxy(0,14);   
    LCD_PutChar(223);
    LCD_PutChar('C');
    LCD_Gotoxy(1,14);   
    LCD_PutChar('%');
    //---------------------------------------------------------------------------------------
}