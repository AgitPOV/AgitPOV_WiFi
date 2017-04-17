/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

var monMot = "testing"; 

var app = {
    // Application Constructor
    initialize: function() {
        document.addEventListener('deviceready', this.onDeviceReady.bind(this), false);
    },

    // deviceready Event Handler
    //
    // Bind any cordova events here. Common events are:
    // 'pause', 'resume', etc.
    onDeviceReady: function() {
        this.receivedEvent('deviceready');
        // alert("readdy");
        
        document.getElementById('envoiButton').addEventListener('touchend', envoi, false);
        // document.getElementById('envoiButton').addEventListener('touchend', envoi(monMot), false);
    },

    // Update DOM on a Received Event
    receivedEvent: function(id) {
        var parentElement = document.getElementById(id);
        var listeningElement = parentElement.querySelector('.listening');
        var receivedElement = parentElement.querySelector('.received');

        listeningElement.setAttribute('style', 'display:none;');
        receivedElement.setAttribute('style', 'display:block;');

        alert('Received Event: ' + id);
    }


    
};

////////////////////////////////////////////////////////////////////////////////
//                         Envoi au esp8266                                   //
////////////////////////////////////////////////////////////////////////////////

//function envoi(unMot) {
    
function envoi() {
    // alert("test : "+unMot);
     // alert("test : ");
  // from http://stackoverflow.com/questions/8638984/send-post-data-to-php-without-using-an-html-form
  var theForm, newInput1;
  theForm = document.createElement('form');
  theForm.action = 'http://192.168.4.1';
  theForm.method = 'post';
  newInput1 = document.createElement('input');
  newInput1.type = 'hidden';
  newInput1.name = 'AgitPOV';
  newInput1.value = 'AAAA';
  //   console.log(unMot);
  // newInput1.value = unMot;
  theForm.appendChild(newInput1);
  document.getElementById('hidden_form_container').appendChild(theForm);
    
  theForm.submit();
   //  alert("le mot est : "+unMot);
  
  }

app.initialize();
    