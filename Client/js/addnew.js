addEventListener("DOMContentLoaded", (event) => {
    
    let addFolderButton = document.getElementById('button-add-folder');

    addFolderButton.addEventListener('click', (e) => {

        e.preventDefault();

        let portNumber = sessionStorage.getItem('portNumber');

        console.log(`port from external is = ${portNumber}`);
        
        fetch(`http://localhost:${portNumber}/getEventsOnFolderTree`)
            .then(response => response.json())
            .then(data => console.log(data))
            .catch(error => console.error('Error:', error));
    });

});