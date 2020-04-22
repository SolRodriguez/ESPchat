def request_handler(request):
    if request['method'] == 'GET':
        pass

    if request['method'] == 'POST':
        #obtain byte data
        data = request['data']

        #decode and convert to string to break up
        d = data.decode('utf-8')
        split1 = d.split("=")
        #['user', 'khernan5&img', '(D$\\x18D\\x18d\\xc5\\x01\\x80\\)']

        split2 = split1[1].split("&")
        #['khernan5', 'img']

        username = split2[0]
        img_data = split1[2]  
        
        ##unsure as to how the actual data is affected due to conversion. That is my conversion was done correctly
    
        
        return (username ,img_data)

        