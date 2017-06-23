//
//  ViewController.swift
//  AgitPOV
//
//  Created by Alexandre Castonguay on 17-04-19.
//  Copyright (c) 2017 Alexandre Castonguay. All rights reserved.
//

import UIKit

class ViewController: UIViewController,UITextFieldDelegate {
    
    //MARK: Properties
    
    @IBOutlet weak var motTextField: UITextField!
    
    @IBOutlet weak var motNameLabel: UILabel!
    override func viewDidLoad() {
        super.viewDidLoad()
        // Handle the text field’s user input through delegate callbacks.
        motTextField.delegate = self
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    //MARK: UITextFieldDelegate
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        // Hide the keyboard.
        textField.resignFirstResponder()
        return true
    }
    
    func textFieldDidEndEditing(_ textField: UITextField) {
        motNameLabel.text = textField.text
        envoi(mot: textField.text!)
    }
    
    func envoi(mot: String) {
        let leMot = "Hello, " + mot + "!"
        print (leMot)
        var request = URLRequest(url: URL(string: "http://192.168.4.1/")!)
        request.httpMethod = "POST"
        let postString = "AgitPOV="+mot
        request.httpBody = postString.data(using: .utf8)
        let task = URLSession.shared.dataTask(with: request) { data, response, error in
            guard let data = data, error == nil else {                       // check for fundamental networking error
                print("error=\(error)")
                return
            }
            
            
            
            if let httpStatus = response as? HTTPURLResponse, httpStatus.statusCode != 200 {       // check for http errors
                print("statusCode should be 200, but is \(httpStatus.statusCode)")
                print("response = \(response)")
            }
            
            let responseString = String(data: data, encoding: .utf8)
            print("responseString = \(responseString)")
        }
        task.resume()
        
        
    }
    
    
    // add send button
    //MARK: Actions
    
    
    /*
     import SystemConfiguration.CaptiveNetwork
     
     internal class SSID {
     class func fetchSSIDInfo() ->  String {
     var currentSSID = ""
     if let interfaces:CFArray = CNCopySupportedInterfaces() {
     for i in 0..<CFArrayGetCount(interfaces){
     let interfaceName: UnsafeRawPointer = CFArrayGetValueAtIndex(interfaces, i)
     let rec = unsafeBitCast(interfaceName, to: AnyObject.self)
     let unsafeInterfaceData = CNCopyCurrentNetworkInfo("\(rec)" as CFString)
     if unsafeInterfaceData != nil {
     
     let interfaceData = unsafeInterfaceData! as Dictionary!
     currentSSID = ((interfaceData as? [String : AnyObject])?["SSID"])! as! String
     
     }
     }
     }
     return currentSSID
     }
     }
     */
    
}

