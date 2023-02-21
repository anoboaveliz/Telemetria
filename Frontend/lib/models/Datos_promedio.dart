
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

class Datos_promedio {
  final DateTime fecha;
  final String temp;
  final String hum;
   Datos_promedio({required this.fecha, required this.temp, required this.hum});

   /*factory Datos_temp_final.fromJson(Map<String, dynamic>json){
     return Datos_temp_final(
        tipo: json["tipo"],
        valor: json["valor"],
      )};*/
}













