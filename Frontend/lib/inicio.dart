import 'package:flutter/material.dart';
import 'package:tempapp/menu.dart';
class inicio extends StatefulWidget {

  @override
  State<inicio> createState() => _inicioState();
}

class _inicioState extends State<inicio> {
  TextEditingController email= new TextEditingController();
  TextEditingController password= new TextEditingController();
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
          color: Colors.black12,
        ),
        padding: EdgeInsets.only(left: 20, right: 20),

        child: Center(
          child: Column(
            children: [
              SizedBox(height: 50),
              Text("Inicio de sesion", style: TextStyle(
                color: Colors.white,
                fontSize: 30,
              ),),
              SizedBox(height: 50),
              TextField(
                controller: email,
                decoration: InputDecoration(
                  hintText: "Nombre de usuario",
                  hintStyle: TextStyle(color: Colors.white),
                  enabledBorder: OutlineInputBorder(
                    borderSide: const BorderSide(color: Colors.white, width: 2.0),

                  )

                ),
              ),
              SizedBox(height: 50),
              TextField(
                controller: password,
                obscureText: true,
                decoration: InputDecoration(
                    hintText: "Contraseña",
                    hintStyle: TextStyle(color: Colors.white),
                    enabledBorder: OutlineInputBorder(
                      borderSide: const BorderSide(color: Colors.white, width: 2.0),

                    )

                ),
              ),
              Container(
                margin: EdgeInsets.only(top: 70),
                width: 200,
                decoration: BoxDecoration(
                    color: Colors.white38,
                    borderRadius: BorderRadius.circular(10)
                ),
                child: FlatButton(
                  child: Text("Continuar", style: TextStyle(color: Colors.white, fontSize: 20),),
                  onPressed: (){
                    Navigator.push(context, MaterialPageRoute(builder: (_)=>menu()));

                  },

                ),
              ),
              SizedBox(height: 20),
              //Text("Olvidaste tu contraseña?", style: TextStyle(
                //color: Colors.white,
                //fontSize: 15,
              //),),

            ],
          ),
        ),
      ),
    );
  }
}
