

import 'dart:async';
import 'dart:convert';
import 'dart:math';

import 'package:tempapp/models/Datos_temp_final.dart';
import 'package:workmanager/workmanager.dart';
import 'package:flutter/material.dart';
import 'package:flutter_echarts/flutter_echarts.dart';
import 'package:http/http.dart' as http;


// Otro inicio



//-----------------------------------------
//const taName = "tarea1";
Datos_temp_final v1 = Datos_temp_final(tipo: "num", valor: "20.5");
Datos_temp_final v2 = Datos_temp_final(tipo: "num", valor: "35.5");
Datos_temp_final v3= Datos_temp_final(tipo: "num", valor: "20.5");
Datos_temp_final v4 = Datos_temp_final(tipo: "num", valor: "20.5");
Datos_temp_final v5 = Datos_temp_final(tipo: "num", valor: "20.5");

List<Datos_temp_final> data = [v1,v2,v3,v4,v5];
List<num> dato = [22.0, 23.0,22.8,21.90,23];
List<Color> colores = [Color(0xffDCDCDC),Color(0xffDCDCDC),Color(0xffDCDCDC),Color(0xffDCDCDC),Color(0xffDCDCDC)];

//List<List<num>> dato = [];
Future<List<Datos_temp_final>> fetchPost1() async {
  var registro = <Datos_temp_final>[];
  for (var i =0 ; i<7; i++) {
    var j = i +1;
    String url="http://200.126.14.228:3826/v2/entities/Nodo" + j.toString();
    final response =
    await http.get(Uri.parse(url));
    var data = jsonDecode(response.body);
    if (response.statusCode == 200) {
      String t = (data["temperatura"]["type"]).toString();
      String v = (data["temperatura"]["value"]).toString();
      registro.add(Datos_temp_final(tipo: t, valor: v));
    } else {
      throw Exception("Fallo la conexion");
    }
  }
  return registro;
}



class maps extends StatefulWidget {

  @override
  State<maps> createState() => _mapsState();

}

class _mapsState extends State<maps> {

    //[[9,1,23.5],[23,1,27.0],[66,1,21.0],[50,100,24.5],[15,100,26.0],[5,100,24.0],[11,9,22.0]];


    @override
    void initState() {
      // TODO: implement initState
      super.initState();
      setState((){
        fetchPost1().then((value){
          data = [];
          data.addAll(value);

        });
        dato = [];
        dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor),];

      });


    }


    @override
  Widget build(BuildContext context) {

    final screenSize = MediaQuery.of(context).size;
    final sw = screenSize.width;
    final sh = screenSize.height;

    //List<num> dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor),double.parse(data[5].valor)];



    /*Timer miTimer = Timer.periodic(Duration(seconds:20), (timer){
      print ("realixo tarea");
      //data = [];
      //dato = [];
      fetchPost1().then((value){
        data = [];
        data.addAll(value);
      });
      dato = [];
      dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor),double.parse(data[5].valor)];

     // print(data);
      //print(dato);

    });*/
    //List<List<num>> dato = [[9,1,double.parse(data[0].valor)],[23,1,27.0],[66,1,21.0],[50,100,24.5],[15,100,26.0],[5,100,24.0],[11,9,22.0]];


    return Scaffold(
      backgroundColor: Colors.black,
      appBar: AppBar(
        shadowColor: Colors.white,
        backgroundColor: Colors.white10,
        foregroundColor: Colors.white,
      ),

      body: Container(
          decoration: BoxDecoration(
            image: DecorationImage(
              image: AssetImage("assets/Plano.png"),
              fit: BoxFit.cover,
            ),
          ),
          height: sh,
          width: sw,
          child: Stack(
            children: <Widget>[
              //NODO 3
              new Positioned(
                //left: 0.1,
                right: 40,
                top: 85,

                child: SizedBox(
                  width: 70,
                  height: 70,

                  child: Card(
                    color: colores[2].withOpacity(0.4),
                    child:Align(
                      alignment: Alignment(0.1, 0.9),
                      child: Text(dato[2].toStringAsFixed(2) + "°C", style: TextStyle( color: Colors.black, fontSize: 12),),
                    ),
                     ),
                ),
              ),


              Positioned(
                //left: 0.1,
                right: 25,
                top: 70,

                child: SizedBox(
                  width: 100,
                  height: 100,

                  child: Card(
                    color: colores[2].withOpacity(0.2),
                    child: IconButton(
                      icon: const Icon(Icons.ad_units),
                      onPressed: (){
                        setState((){
                          fetchPost1().then((value){
                            data = [];
                            data.addAll(value);
                          });
                          dato = [];
                          dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor)];
                          if (dato[2] <= 0){
                            colores[2]= Color(0xffDCDCDC);
                          }else if (dato[2] > 1 && dato[2]< 10){
                            colores[2]= Color(0xff20B2AA);
                          }else if (dato[2] > 10 && dato[2]< 20){
                            colores[2]= Color(0xff87CEFA);
                          }else if (dato[2] > 20 && dato[2]< 25){
                            colores[2]= Color(0xffFFA500);
                          } else if (dato[2] > 25 && dato[2]< 30){
                            colores[2]= Color(0xffFF4500);
                          } else{
                            colores[2]= Color(0xffFF0000);
                          }
                        });
                        print(data[0].valor);
                        print(dato);



                      },

                    ),
                  ),
                ),
              ),
              //------------------------------------------------------------------------ NODO 4
              new Positioned(
                //left: 0.1,
                right: 140,
                top: 85,

                child: SizedBox(
                  width: 70,
                  height: 70,

                  child: Card(
                    color: colores[3].withOpacity(0.4),
                    child: Align(
                      alignment: Alignment(0.1, 0.9),
                      child: Text(dato[3].toStringAsFixed(2) + "°C", style: TextStyle( color: Colors.black, fontSize: 12),),
                    ),
                  ),
                ),
              ),
              new Positioned(
                //left: 0.1,
                right: 125,
                top: 70,

                child: SizedBox(
                  width: 100,
                  height: 100,

                  child: Card(
                    color: colores[3].withOpacity(0.2),
                    child:  IconButton(
                      icon: const Icon(Icons.ad_units),
                      onPressed: (){
                        setState((){
                          fetchPost1().then((value){
                            data = [];
                            data.addAll(value);
                          });
                          dato = [];
                          dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor)];
                          if (dato[3] <= 0){
                            colores[3]= Color(0xffDCDCDC);
                          }else if (dato[3] > 1 && dato[3]< 10){
                            colores[3]= Color(0xff20B2AA);
                          }else if (dato[3] > 10 && dato[3]< 20){
                            colores[3]= Color(0xff87CEFA);
                          }else if (dato[3] > 20 && dato[3]< 25){
                            colores[3]= Color(0xffFFA500);
                          } else if (dato[3] > 25 && dato[3]< 30){
                            colores[3]= Color(0xffFF4500);
                          } else{
                            colores[3]= Color(0xffFF0000);
                          }
                        });

                      },

                    ),

                  ),
                ),
              ),
              //---------------------------------------------------------------------------
             /* new Positioned(
                //left: 0.1,
                right: 246,
                top: 85,

                child: SizedBox(
                  width: 70,
                  height: 70,

                  child: Card(
                    color: Colors.teal.withOpacity(0.4),
                    child: IconButton(
                      icon: const Icon(Icons.ad_units),
                      onPressed: (){

                      },
                    ),
                  ),
                ),

              ),
              new Positioned(
                //left: 0.1,
                right: 230,
                top: 70,

                child: SizedBox(
                  width: 100,
                  height: 100,

                  child: Card(
                    color: Colors.teal.withOpacity(0.2),
                    child: Align(
                      alignment: Alignment(0.1, 0.6),
                      child: Text(dato[3].toStringAsFixed(2) + "°C", style: TextStyle( color: Colors.white, fontSize: 12),),
                    ),

                  ),
                ),

              ),*/
              //----------------------------------------------------------------- NODO 1
              new Positioned(
                //left: 0.1,
                right: 175,
                top: 550,

                child: SizedBox(
                  width: 70,
                  height: 70,

                  child: Card(
                    color: colores[0].withOpacity(0.4),
                    child: Align(
                      alignment: Alignment(0.1, 0.9),
                      child: Text(dato[0].toStringAsFixed(2) + "°C", style: TextStyle( color: Colors.black, fontSize: 12),),
                    ),
                  ),
                ),
              ),
              new Positioned(
                //left: 0.1,
                right: 160,
                top: 535,

                child: SizedBox(
                  width: 100,
                  height: 100,

                  child: Card(
                    color: colores[0].withOpacity(0.2),
                    child: IconButton(
                      icon: const Icon(Icons.ad_units),
                      onPressed: (){
                        setState((){
                          fetchPost1().then((value){
                            data = [];
                            data.addAll(value);
                          });
                          dato = [];
                          dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor),];
                          if (dato[0] <= 0){
                            colores[0]= Color(0xffDCDCDC);
                          }else if (dato[0] > 1 && dato[0]< 10){
                            colores[0]= Color(0xff20B2AA);
                          }else if (dato[0] > 10 && dato[0]< 20){
                            colores[0]= Color(0xff87CEFA);
                          }else if (dato[0] > 20 && dato[0]< 25){
                            colores[0]= Color(0xffFFA500);
                          } else if (dato[0] > 25 && dato[0]< 30){
                            colores[0]= Color(0xffFF4500);
                          } else{
                            colores[0]= Color(0xffFF0000);
                          }
                        });

                      },

                    ),

                  ),
                ),
              ),
              //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''' NODO5

              new Positioned(
                //left: 0.1,
                right: 95,
                top: 550,

                child: SizedBox(
                  width: 70,
                  height: 70,

                  child: Card(
                    color: colores[4].withOpacity(0.5),
                    child: Align(
                      alignment: Alignment(0.1, 0.9),
                      child: Text(dato[4].toStringAsFixed(2) + "°C", style: TextStyle( color: Colors.black, fontSize: 12),),
                    ),
                  ),
                ),
              ),
              new Positioned(
                //left: 0.1,
                right: 90,
                top: 535,

                child: SizedBox(
                  width: 80,
                  height: 100,

                  child: Card(
                    color: colores[4].withOpacity(0.2),
                    child: IconButton(
                      icon: const Icon(Icons.ad_units),
                      onPressed: (){
                        setState((){
                          fetchPost1().then((value){
                            data = [];
                            data.addAll(value);
                          });
                          dato = [];
                          dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor),];
                          if (dato[4] <= 0){
                            colores[4]= Color(0xffDCDCDC);
                          }else if (dato[4] > 1 && dato[4]< 10){
                            colores[4]= Color(0xff20B2AA);
                          }else if (dato[4] > 10 && dato[4]< 20){
                            colores[4]= Color(0xff87CEFA);
                          }else if (dato[4] > 20 && dato[4]< 25){
                            colores[4]= Color(0xffFFA500);
                          } else if (dato[4] > 25 && dato[4]< 30){
                            colores[4]= Color(0xffFF4500);
                          } else{
                            colores[4]= Color(0xffFF0000);
                          }
                        });

                      },

                    ),

                  ),
                ),
              ),
              //-------------------------------------------NODO 2
              new Positioned(
                //left: 0.1,
                right: 10,
                top: 550,

                child: SizedBox(
                  width: 70,
                  height: 70,

                  child: Card(
                    color: colores[1].withOpacity(0.5),
                    child:Align(
                      alignment: Alignment(0.1, 0.9),
                      child: Text(dato[1].toStringAsFixed(2) + "°C", style: TextStyle( color: Colors.black, fontSize: 12),),
                    ),
                  ),
                ),
              ),
              new Positioned(
                //left: 0.1,
                right: 0.0,
                top: 535,

                child: SizedBox(
                  width: 100,
                  height: 100,

                  child: Card(
                    color: colores[1].withOpacity(0.2),
                    child: IconButton(
                      icon: const Icon(Icons.ad_units),
                      onPressed: (){
                        setState((){
                          fetchPost1().then((value){
                            data = [];
                            data.addAll(value);
                          });
                          dato = [];
                          dato = [double.parse(data[0].valor),double.parse(data[1].valor),double.parse(data[2].valor),double.parse(data[3].valor),double.parse(data[4].valor),];
                          if (dato[1] <= 0){
                            colores[1]= Color(0xffDCDCDC);
                          }else if (dato[1] > 1 && dato[1]< 10){
                            colores[1]= Color(0xff20B2AA);
                          }else if (dato[1] > 10 && dato[1]< 20){
                            colores[1]= Color(0xff87CEFA);
                          }else if (dato[1] > 20 && dato[1]< 25){
                            colores[1]= Color(0xffFFA500);
                          } else if (dato[1] > 25 && dato[1]< 30){
                            colores[1]= Color(0xffFF4500);
                          } else{
                            colores[1]= Color(0xffFF0000);
                          }
                        });

                      },

                    ),

                  ),
                ),
              ),
            ],

          ),

        ),




    );
  }





}


