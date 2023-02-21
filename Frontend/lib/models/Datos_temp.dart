

import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

class Datos_temp {

  final  int userId;
  final int id;
  final String title;
  final String body;

  Datos_temp({required this.userId, required this.id, required this.title, required this.body});

  factory Datos_temp.fromJson(Map<String, dynamic> json) {
    return Datos_temp(
      userId: json['userId'],
      id: json['id'],
      title: json['title'],
      body: json['body'],
    );
  }










}