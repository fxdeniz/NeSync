addEventListener("DOMContentLoaded", (event) => {
    
    let connectButton = document.getElementById('button-connect');

    connectButton.addEventListener('click', (e) => {

        e.preventDefault();

        let portNumber = document.getElementById('input-port').value;
        
        fetch(`http://localhost:${portNumber}/getEventsOnFolderTree`)
            .then(response => response.json())
            .then(data => console.log(data))
            .catch(error => console.error('Error:', error));
    });

});