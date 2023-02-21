import 'package:flutter/material.dart';
import 'package:tempapp/historial.dart';
import 'package:tempapp/maps.dart';
import 'package:tempapp/modo.dart';
import 'package:workmanager/workmanager.dart';

class menu extends StatefulWidget {


  @override
  State<menu> createState() => _menuState();
}

class _menuState extends State<menu> {

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
              //SizedBox(height:50),
              //Text("Elige una opcion", style: TextStyle(
                //color: Colors.white,
                //fontSize: 40,
              //),),
              SizedBox(height:35),
              Container(
                margin: EdgeInsets.only(top: 80),
                width: 200,
                height: 65,
                decoration: BoxDecoration(
                    color: Colors.teal,
                    borderRadius: BorderRadius.circular(10)
                ),
                child: FlatButton(
                  child: Text("Mapa de calor", style: TextStyle(color: Colors.white, fontSize: 20),),
                  onPressed: (){
                    //Workmanager().registerPeriodicTask(taName, taName, initialDelay: Duration.zero,);
                    Navigator.push(context, MaterialPageRoute(builder: (_)=>maps()));


                  },

                ),
              ),
              Container(
                margin: EdgeInsets.only(top: 80),
                width: 200,
                height: 65,
                decoration: BoxDecoration(
                    color: Colors.teal,
                    borderRadius: BorderRadius.circular(10)
                ),
                child: FlatButton(
                  child: Text("Historial", style: TextStyle(color: Colors.white, fontSize: 20),),
                  onPressed: (){
                    //Workmanager().cancelByUniqueName(taName);
                    Navigator.push(context, MaterialPageRoute(builder: (_)=>historial()));
                  },

                ),
              ),
              Container(
                margin: EdgeInsets.only(top: 80),
                width: 200,
                height: 65,
                decoration: BoxDecoration(
                    color: Colors.teal,
                    borderRadius: BorderRadius.circular(10)
                ),
                child: FlatButton(
                  child: Text("Configurar Modo", style: TextStyle(color: Colors.white, fontSize: 20),),
                  onPressed: (){
                    //Workmanager().cancelByUniqueName(taName);
                    Navigator.push(context, MaterialPageRoute(builder: (_)=>modo()));

                  },

                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
