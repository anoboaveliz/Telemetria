import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:http/http.dart';
import 'package:intl/intl.dart';
import 'package:tempapp/models/Datos_promedio.dart';
import 'package:tempapp/models/Datos_temp.dart';
import 'dart:convert' as convert;
import 'package:http/http.dart' as http;


class historial extends StatefulWidget {


  @override
  State<historial> createState() => _historialState();
}

class _historialState extends State<historial> {

   List<Datos_temp> data = <Datos_temp>[];
   List<Datos_promedio> dato_pro = <Datos_promedio>[];
   //List<Datos_promedio> registros = <Datos_promedio>[];
   List<dynamic> fecha = [];
   List<dynamic> tempe = [];
   List<dynamic> hume = [];
   List<DateTime> fechas = [];
   List<double> temps=[];
   List<double> hums=[];
   //String url='https://jsonplaceholder.typicode.com/posts';
   String url2='http://200.126.14.228:3868/v2/entities/Promedio';



  Future <List<Datos_promedio>> prueba(int va) async{
    final url = Uri.parse(url2);
    Response response = await get(url);
    var data = jsonDecode(response.body);
    var registros = <Datos_promedio>[];
    print('Status code: ${response.statusCode}');
    //print('Headers: ${response.headers}');
    //print (data["attributes"][1]["values"]);

    if (response.statusCode == 200){

       fecha = [];
       tempe = [];
       hume = [];
       fecha.addAll(data["attributes"][1]["values"]);
       tempe.addAll(data["attributes"][2]["values"]);
       hume.addAll(data["attributes"][5]["values"]);
       final fr = fecha.reversed.toList();
       final tr = tempe.reversed.toList();
       final hr = hume.reversed.toList();
      //print(data);
       //print(tempe);
       //print(hume);
       for (var i = 0; i <= 13; i++) {

         DateTime now = DateTime.parse(fr[i].split(".")[0]);


         String t = (tr[i].toString()).split(".")[0]+ "." + (tr[i].toString()).split(".")[1][0];
         String h = (hr[i].toString()).split(".")[0] + "." + (hr[i].toString()).split(".")[1][0];
         //print(now);
         //print(t);
         //print(h);

         Datos_promedio entidad = new Datos_promedio(fecha: now, temp: t, hum: h);
         registros.add(entidad);
         //print(entidad.temp);
        //print(entidad.temp);
        // print(entidad.hum);
         //print(registros);
       }
       print(registros);
    }else {
      throw Exception("Fallo la conexion");
    }

    return registros;


  }



  @override
  void initState() {
    // TODO: implement initState
    super.initState();

  }

  //var _lista= ["Por Hora","Por Dia","Por Mes"];
   var _lista= ["Historial"];
  String _opcion ="Seleccione una opcion";
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      resizeToAvoidBottomInset: true,
      backgroundColor: Colors.white,
      appBar: AppBar(
        shadowColor: Colors.white,
        backgroundColor: Colors.white10,
        foregroundColor: Colors.white,
      ),
      body: Container(
        decoration: BoxDecoration(
          color: Colors.black12,
        ),
        padding: EdgeInsets.only(left: 20, right: 20),

        child: Center(
          child: Column(
            children: [
              //SizedBox(height:50),
              DropdownButton(
                  icon: const Icon(Icons.arrow_downward, color: Colors.teal, size: 20,),
                  underline: Container(
                    height: 1,
                    color: Colors.teal,
                  ),
                  isExpanded: true,
                  items: _lista.map((String a){
                    return DropdownMenuItem(
                        value: a,
                        child: Text(a, style:TextStyle( color: Colors.teal, fontSize: 15) ,));
                  }).toList(),
                  onChanged: (_value) =>{
                    setState((){
                      _opcion = _value.toString();
                      if(_opcion == 'Historial'){
                        prueba(1).then((value){
                          setState((){
                            dato_pro = [];
                            dato_pro.addAll(value);
                            //print(dato_pro);
                          });
                        });


                        //print(dato_pro);
                      } else if (_opcion == 'Historials'){

                      prueba(1);




                      }else if (_opcion == 'Por Mes'){

                        prueba(1).then((value){
                          setState((){
                            dato_pro = [];
                            dato_pro.addAll(value);
                          });
                        });


                      }else{
                       /* fetchPost().then((value){
                          setState((){
                            data.addAll(value);
                          });
                        });*/
                        prueba(1).then((value){
                          setState((){
                            dato_pro.addAll(value);
                          });
                        });
                      }
                    })
                  },
                  hint: Text(_opcion, style: TextStyle(
                    color: Colors.black,
                    fontSize: 15,
                  ),),
              ),
              SizedBox(height:10),
              ListView.builder(
                  scrollDirection: Axis.vertical,
                  shrinkWrap: true,
                  itemCount: dato_pro.length,
                  itemBuilder: (BuildContext context, int index){

                    return Card(
                      color: Colors.teal,
                      elevation: 3.5,
                      borderOnForeground: true,
                      child: Padding(
                        padding: const EdgeInsets.all(9.0),
                        child: Text((dato_pro[index].fecha).toString() + "         " + (dato_pro[index].hum).toString()+" °C" + "         " + (dato_pro[index].temp).toString()+" %", style: TextStyle( color: Colors.black, fontSize: 15),),


                      ),

                    );

                  },
              ),



              

            ],
          ),
        ),
      ),
    );
  }

}

