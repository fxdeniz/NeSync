async function postJSON(targetUrl, requestBody) {
    try {
      const response = await fetch(targetUrl, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(requestBody),
      });
  
      const result = await response.text();
      console.log("Success:", result);
    } catch (error) {
      console.error("Error:", error);
    }
  }

  async function fetchJSON(targetUrl) {
    try {
      const response = await fetch(targetUrl);
      
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      
      const result = await response.json();
      
      return result;

    } catch (error) {
      console.error('There was a problem with the fetch operation:', error);
    }
  }

document.addEventListener("DOMContentLoaded", async (event) => {

    let inputCurrentPath = document.getElementById('input-current-path');
    let buttonPrev = document.getElementById('button-prev');
    let buttonNext = document.getElementById('button-next');
    const buttonAddNewFolder = document.getElementById('button-add-new-folder');

    buttonPrev.disabled = true;
    buttonNext.disabled = true;
    inputCurrentPath.value = '/';
    inputCurrentPath.readOnly = true;

    buttonAddNewFolder.addEventListener('click', async () => {

        let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
        const selectedFolderPath = await window.fileExplorerApi.showFolderSelectDialog();

        if(selectedFolderPath) {
            console.log(selectedFolderPath);
            let pathTokens = await window.pathApi.splitPath(selectedFolderPath);
            requestBody["symbolFolderPath"] = inputCurrentPath.value + pathTokens.pop();
            requestBody["userFolderPath"] = selectedFolderPath;

            postJSON('http://localhost:1234/addNewFolder', requestBody);
        }
    });

    let folderJson = await fetchJSON(`http://localhost:1234/getFolderContent?symbolPath=${inputCurrentPath.value}`);

    folderJson['childFolders'].forEach(currentFolder => {
      let tableExplorerBody = document.querySelector('#table-explorer tbody');

      let row = document.createElement('tr');
      let colName = document.createElement('td');
      let colLocation = document.createElement('td');
      
      colName.innerText = currentFolder['suffixPath'];
      colLocation.innerText = currentFolder['userFolderPath'];

      row.appendChild(colName);
      row.appendChild(colLocation);

      tableExplorerBody.appendChild(row);
    });

  });
  