import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
class modo extends StatefulWidget {


  @override
  State<modo> createState() => _modoState();
}

class _modoState extends State<modo> {
  double raitingM = 23.0;
  double raitingMI = 23.0;
  String url='http://200.126.14.228:3826/v2/entities/Configuracion/attrs/';
  String url2='http://200.126.14.228:3826/v2/entities/Configuracion/attrs/';

  Future<void> fet(double max, double min) async {
    final hea = {"Content-type": "application/json"};
    final json = '{"tempMaxTermostato":{"type":"Number", "value": ${max}, "metadata":{}},"tempMinTermostato":{"type":"Number", "value": ${min}, "metadata":{}}}';
    final response = await http.patch(Uri.parse(url),headers: hea, body: json);
    print("status code: ${response.statusCode}");
    print('Body: ${response.body}');


  }
  Future<void> fet2(String va) async {
    final heas = {"Content-type": "application/json"};
    final jsons = '{"estado": {"type":"Number", "value":${va},"metadata":{}}}';
    final response = await http.patch(Uri.parse(url2),headers: heas, body: jsons);
    print("status code: ${response.statusCode}");
    print('Body: ${response.body}');

  }
  void showAlert(){
    showDialog(context: context,
        builder: (buildcontext){
      return AlertDialog(
        title: Text("Configuracion Invalida", style: TextStyle(
          color: Colors.red,
          fontSize: 25,
        ),),
        content: Text("Min °C >= Max °C", style: TextStyle(
          color: Colors.teal,
          fontSize: 20,
        ),),
        actions:<Widget> [
          RaisedButton(
              child: Text("CERRAR",style: TextStyle(color: Colors.white)),
              splashColor: Colors.white,
              color: Colors.teal,
              onPressed: () {Navigator.of(context).pop();},
          )
        ],
      );
        });
  }
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.black,
      appBar: AppBar(
        shadowColor: Colors.white,
        backgroundColor: Colors.white10,
        foregroundColor: Colors.white,
      ),
      body: Container(
        decoration: BoxDecoration(
          color: Colors.white,
        ),
        padding: EdgeInsets.only(left: 20, right: 20),

        child: Center(
          child: Column(
            children: [
              SizedBox(height:100),
              Text("Max °C ", style: TextStyle(
                color: Colors.teal,
                fontSize: 30,
              ),
              ),
              SizedBox(height:20),
              Slider(
                  activeColor: Colors.teal ,
                  thumbColor: Colors.teal,
                  label: raitingM.round().toString(),
                  value: raitingM,
                  divisions: 5,
                  min: 21,
                  max: 27,

                  onChanged: (double value){
                    setState((){
                      raitingM = value;
                    });
                  },
              ),
              SizedBox(height:35),
              Text("Min °C", style: TextStyle(
                color: Colors.teal,
                fontSize: 30,
              ),),
              SizedBox(height:20),
              Slider(
                activeColor: Colors.teal ,
                thumbColor: Colors.teal,
                label: raitingMI.round().toString(),
                value: raitingMI,
                divisions: 5,
                min: 21,
                max: 27,

                onChanged: (double value){
                  setState((){
                    raitingMI = value;
                  });
                },
              ),
              SizedBox(height:35),
              Container(
                child:
                RaisedButton(
                  disabledColor: Colors.amber,
                  child: Text("Enviar"),
                  splashColor: Colors.white,
                  color: Colors.teal,
                  onPressed: ()  {
                    print("Hola Raised Button_enviar FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
                    if (raitingMI >= raitingM){
                      print("ESTA MALLLL AFEDO");
                      showAlert();
                    }
                    fet(raitingM, raitingMI);
                  },
                ),
              ),
              SizedBox(height:35),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceAround,
                children: [
                  Container(
                    child:
                    RaisedButton(
                      disabledColor: Colors.amber,
                      child: Text("Encender"),
                      splashColor: Colors.white,
                      color: Colors.teal,
                      onPressed: ()  {
                        print("Hola Raised Button_enviar FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
                        fet2("1.0");
                      },
                    ),
                  ),
                  Container(
                    child: RaisedButton(
                      disabledColor: Colors.amber,
                      child: Text("Apagar"),
                      splashColor: Colors.white,
                      color: Colors.teal,
                      onPressed: ()  {
                        print("Hola Raised Button_enviar FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
                        fet2("0.0");
                      },
                    ),
                  )
                ],
              )



            ],
          ),
        ),
      ),
    );
  }
}
