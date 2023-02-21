import 'package:flutter/material.dart';
import 'package:atlas/atlas.dart';
import 'package:workmanager/workmanager.dart';

import 'maps.dart';
import 'menu.dart';


void main() async{
  //WidgetsFlutterBinding.ensureInitialized();
  //await Workmanager().initialize(callbackDispatcher, isInDebugMode: true,);
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        // This is the theme of your application.
        //
        // Try running your application with "flutter run". You'll see the
        // application has a blue toolbar. Then, without quitting the app, try
        // changing the primarySwatch below to Colors.green and then invoke
        // "hot reload" (press "r" in the console where you ran "flutter run",
        // or simply save your changes to "hot reload" in a Flutter IDE).
        // Notice that the counter didn't reset back to zero; the application
        // is not restarted.
        primarySwatch: Colors.blue,
      ),
      home: _inicio(),
    );
  }
}
class _inicio extends StatefulWidget {
  const _inicio({Key? key}) : super(key: key);

  @override
  State<_inicio> createState() => _inicioState();
}

class _inicioState extends State<_inicio> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        decoration: BoxDecoration(
          image: DecorationImage(
            image: AssetImage("assets/p1.jpg"),
            fit: BoxFit.cover,
          )
        ),
        child: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              SizedBox(height: 470),
              Image.asset("assets/logoq.png"),
              SizedBox(height: 50),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: <Widget> [
                  new RaisedButton(
                      color: Colors.teal,
                      textColor: Colors.white,

                      child: Text(
                        "Empezar", style: TextStyle(
                        fontSize: 20.0,
                      ),
                      ),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(19),
                        side: BorderSide(color: Colors.white30),

                      ),
                      onPressed: (){
                        Navigator.push(
                            context,
                            MaterialPageRoute(builder: (_)=>menu()),
                        );
                      },
                  ),

                ],
              )
            ],
          ),
        ),
      ),
    );

  }
}



