import sys
import sqlite3
from PyQt5 import QtWidgets,QtSerialPort
from PyQt5.QtGui import QPalette
from PyQt5.QtCore import Qt,QIODevice,QTimer
import time
import datetime
import locale
import os

class Window2(QtWidgets.QWidget):
    def __init__(self):
        super(Window2, self).__init__()
        self.setWindowTitle("Pencere")

class Window(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        locale.setlocale(locale.LC_ALL,"")
        self.unit_ui()

    def unit_ui(self):
        self.text_area=QtWidgets.QTextEdit()
        self.coms=QtWidgets.QPushButton("COM")
        self.bound=QtWidgets.QPushButton("BoudRates")
        self.data=QtWidgets.QPushButton("DataBits")
        self.stop=QtWidgets.QPushButton("Stop Bits")
        self.clear=QtWidgets.QPushButton("Clear")
        self.send=QtWidgets.QPushButton("Send Data")
        self.connection=QtWidgets.QPushButton("Connect")
        self.disconnect=QtWidgets.QPushButton("Disconnect")
        self.lineEdit=QtWidgets.QLineEdit()
        self.nfcsend=QtWidgets.QPushButton("Write A Record to NFC")
        self.nfclineEdit=QtWidgets.QLineEdit()
        self.writeFile=QtWidgets.QPushButton("Open Data Base")
        self.clearFile=QtWidgets.QPushButton("Clear Data Base")
        self.database=QtWidgets.QPushButton("Data Base")


        self.v_box=QtWidgets.QVBoxLayout()
        self.h_box = QtWidgets.QHBoxLayout()
        self.h1_box=QtWidgets.QHBoxLayout()
        self.h2_box=QtWidgets.QHBoxLayout()


        self.h_box.addStretch(1)

        self.h_box.addWidget(self.coms)
        self.h_box.addWidget(self.bound)
        self.h_box.addWidget(self.data)
        self.h_box.addWidget(self.stop)
        self.h_box.addWidget(self.connection)
        self.h_box.addWidget(self.disconnect)
        self.h_box.addWidget(self.clear)
        self.h_box.addWidget(self.database)

        self.h1_box.addStretch(1)

        self.h1_box.addWidget(self.send)
        self.h1_box.addWidget(self.lineEdit)
        self.h1_box.addWidget(self.nfcsend)
        self.h1_box.addWidget(self.nfclineEdit)


        self.h2_box.addWidget(self.text_area)
        self.v_box.addLayout(self.h_box)
        self.v_box.addLayout(self.h1_box)
        self.v_box.addLayout(self.h2_box)

        self.setLayout(self.v_box)
        self.clear.clicked.connect(self.clear_text)


    def clear_text(self):
        self.text_area.clear()

class Work(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.window = Window()
        self.qserial =QtSerialPort.QSerialPort()
        self.setCentralWidget(self.window)
        self.menus()
        self.baglanti_Olustur()

    def baglanti_Olustur(self):
        self.baglanti=sqlite3.connect("data_db")
        self.cursor=self.baglanti.cursor()

        self.cursor.execute("Create Table If not exists DATA(TIME TEXT,TEXT TEXT, URL TEXT)")

        self.baglanti.commit()
    def menus(self):
        self.serial_boud=None
        self.serial_data=None
        self.serial_port=None
        self.serial_stop=None
        menu_coms=QtWidgets.QMenu()
        menu_boud=QtWidgets.QMenu()
        menu_data=QtWidgets.QMenu()
        menu_stop=QtWidgets.QMenu()
        menu_file=QtWidgets.QMenu()
        self.window.coms.setMenu(menu_coms)
        self.window.bound.setMenu(menu_boud)
        self.window.data.setMenu(menu_data)
        self.window.stop.setMenu(menu_stop)
        self.window.database.setMenu(menu_file)


        readdata=QtWidgets.QAction("Open Data Base",self)
        cleardata=QtWidgets.QAction("Clear Data Base",self)
        updatedata=QtWidgets.QAction("Update Data Base",self)

        menu_file.addAction(readdata)
        menu_file.addAction(cleardata)
        menu_file.addAction(updatedata)


        ## COMS PARAMETERS ##
        com3=QtWidgets.QAction("COM4",self)
        com4 = QtWidgets.QAction("COM5", self)
        com5 = QtWidgets.QAction("COM6", self)
        com6 = QtWidgets.QAction("COM7", self)
        menu_coms.addAction(com3)
        menu_coms.addAction(com4)
        menu_coms.addAction(com5)
        menu_coms.addAction(com6)

        ## BAUD RATE PARAMETERS ##
        br_96=QtWidgets.QAction("1200",self)
        br_14 = QtWidgets.QAction("2400", self)
        br_19 = QtWidgets.QAction("4800", self)
        br_28 = QtWidgets.QAction("9600", self)
        br_38 = QtWidgets.QAction("19200", self)
        br_57 = QtWidgets.QAction("38400", self)
        br_115 = QtWidgets.QAction("57600", self)
        br_230 = QtWidgets.QAction("115200", self)

        menu_boud.addAction(br_96)
        menu_boud.addAction(br_14)
        menu_boud.addAction(br_19)
        menu_boud.addAction(br_28)
        menu_boud.addAction(br_38)
        menu_boud.addAction(br_57)
        menu_boud.addAction(br_115)
        menu_boud.addAction(br_230)

        ##BYTE PARAMETERS##
        data_5=QtWidgets.QAction("5",self)
        data_6=QtWidgets.QAction("6",self)
        data_7=QtWidgets.QAction("7",self)
        data_8=QtWidgets.QAction("8",self)

        menu_data.addAction(data_5)
        menu_data.addAction(data_6)
        menu_data.addAction(data_7)
        menu_data.addAction(data_8)

        #STOP BITS##
        stop_1=QtWidgets.QAction("1",self)
        stop_2=QtWidgets.QAction("2",self)

        menu_stop.addAction(stop_1)
        menu_stop.addAction(stop_2)

        menu_coms.triggered.connect(self.response_com)
        menu_boud.triggered.connect(self.response_boud)
        menu_data.triggered.connect(self.response_data)
        menu_stop.triggered.connect(self.response_stop)
        self.window.connection.clicked.connect(self.connection)
        self.window.disconnect.clicked.connect(self.disconnection)
        self.window.writeFile.clicked.connect(self.window2)
        self.window.clearFile.clicked.connect(self.clearFile)
        menu_file.triggered.connect(self.response_database)


        self.window.nfcsend.clicked.connect(self.send_NFC_data)

        self.setWindowTitle("UART TERMINAL")
        self.setGeometry(600, 200, 625, 625)
    def response_database(self,action_sqlt):
        if action_sqlt.text()=="Open Data Base":
            self.window2()
        elif action_sqlt.text()=="Clear Data Base":
            self.clearFile()
        elif action_sqlt.text()=="Update Data Base":
            self.updateFile()
    def response_com(self,action):

        self.serial_port=action.text()

    def response_boud(self,action_bound):
        if (action_bound.text()==str(1200)):
            self.serial_boud=self.qserial.BaudRate.Baud1200
        if (action_bound.text() == str(2400)):
            self.serial_boud = self.qserial.BaudRate.Baud2400
        if (action_bound.text() == str(4800)):
            self.serial_boud = self.qserial.BaudRate.Baud4800
        if (action_bound.text() == str(9600)):
            self.serial_boud = self.qserial.BaudRate.Baud9600
        if (action_bound.text() == str(19200)):
            self.serial_boud = self.qserial.BaudRate.Baud19200
        if (action_bound.text() == str(38400)):
            self.serial_boud = self.qserial.BaudRate.Baud38400
        if (action_bound.text() == str(57600)):
            self.serial_boud = self.qserial.BaudRate.Baud57600
        if (action_bound.text() == str(115200)):
            self.serial_boud = self.qserial.BaudRate.Baud115200


    def response_data(self,action_data):

        if (action_data.text() == str(5)):
            self.serial_data = self.qserial.DataBits.Data5

        elif (action_data.text() == str(6)):
            self.serial_data = self.qserial.DataBits.Data6
        elif (action_data.text() == str(7)):
            self.serial_data = self.qserial.DataBits.Data7
        elif (action_data.text() == str(8)):
            self.serial_data = self.qserial.DataBits.Data8

    def response_stop(self,action_stop):
        if(action_stop.text()==str(1)):
            self.serial_stop=self.qserial.StopBits.OneStop
        elif(action_stop.text()==str(2)):
            self.serial_stop= self.qserial.StopBits.TwoStop
    def connection(self):
        if (self.serial_boud==None or self.serial_data==None or self.serial_port==None or self.serial_stop==None) :
            self.window.text_area.setText("Parameter selections are not complete")
        elif (self.serial_boud != None and self.serial_data != None and self.serial_port != None and self.serial_stop!=None):

            self.window.text_area.append("Port Number:{}\nBoud Rate:{}\nByte:{}\nStop bits:{}".format(self.serial_port, self.serial_boud, self.serial_data,self.serial_stop))
            self.qserial.setPortName(self.serial_port)

            self.qserial.setBaudRate(self.qserial.BaudRate.Baud115200)
            self.qserial.setDataBits(self.serial_data)
            self.qserial.setStopBits(self.serial_stop)
            self.qserial.setFlowControl(self.qserial.FlowControl.NoFlowControl)

            self.qserial.open(QIODevice.ReadWrite)
            if(self.qserial.isOpen()!=False):
                self.window.text_area.append("Port is Open. Waiting for incoming Data...\r\n")
                self.qserial.readyRead.connect(self.reading)
                self.window.send.clicked.connect(self.send_data)
        else:
            self.window.text_area.append("Port can not open!\r\n")
    def disconnection(self):
        self.qserial.close()
        if(self.qserial.isOpen()==False):
            self.window.text_area.append("Disconnected...\r\n")
    def reading(self):
        while self.qserial.canReadLine():
            time.sleep(0.25)
            b=bytes(self.qserial.readLine())
            try:
                read_all = b.decode("utf-8")
            except UnicodeDecodeError:
                read_all = str(b)[2:-1]
            self.window.text_area.append(read_all)



    def send_data(self):
        time.sleep(0.25)
        self.qserial.write(self.window.lineEdit.text().encode("utf-8"))
        self.window.lineEdit.clear()
    def send_NFC_data(self):

        self.qserial.write(self.window.nfclineEdit.text().encode("utf-8"))
        self.window.text_area.append("Data to be written to the tag: {} ".format(self.window.nfclineEdit.text()))
        data=self.window.nfclineEdit.text()

        now=datetime.datetime.now()
        a=datetime.datetime.strftime(now,"%d %B %Y %X")

        a=str(a)

        self.window.nfclineEdit.clear()
        self.cursor.execute("Insert into DATA Values(?,?,'NoNe')", (a, data))
        self.baglanti.commit()

    def window2(self):
        #dosya_ismi = QtWidgets.QFileDialog.getOpenFileName(self, "Open", os.getenv("HOME"))
        #print(dosya_ismi)
        #with open(dosya_ismi[0], "r") as file:
            #self.text_area.append(file.read())
        self.cursor.execute("Select * From DATA")
        rows=self.cursor.fetchall()
        if len(rows)==0:
            self.window.text_area.append("Data Base is empty!")
        else:
            self.window.text_area.append("DATE                                      DATA")
            self.window.text_area.append("¯¯¯¯¯¯¯                                  ¯¯¯¯¯¯¯\n ")
            for row in rows:
                self.window.text_area.append("{}             {}".format(row[0],row[1]))

                self.window.text_area.append("\n")
    def clearFile(self):
        self.cursor.execute("Delete From DATA")
        self.baglanti.commit()
    def updateFile(self):
        pass
app = QtWidgets.QApplication(sys.argv)
app.setStyle('Fusion')
palet = QPalette()
palet.setColor(QPalette.ButtonText, Qt.cyan)
palet.setColor(QPalette.Button, Qt.darkBlue)
palet.setColor(QPalette.Text, Qt.yellow)
palet.setColor(QPalette.Base, Qt.black)
app.setPalette(palet)
work=Work()
work.show()
sys.exit(app.exec_())
