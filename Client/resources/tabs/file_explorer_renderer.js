async function postJSON(url, data) {
    try {
      const response = await fetch(url, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(data),
      });
  
      const result = await response.text();
      console.log("Success:", result);
    } catch (error) {
      console.error("Error:", error);
    }
  }

document.addEventListener("DOMContentLoaded", (event) => {

    let inputCurrentPath = document.getElementById('input-current-path');
    let buttonPrev = document.getElementById('button-prev');
    let buttonNext = document.getElementById('button-next');
    const buttonAddNewFolder = document.getElementById('button-add-new-folder');

    buttonPrev.disabled = true;
    buttonNext.disabled = true;
    inputCurrentPath.value = '\\';
    inputCurrentPath.readOnly = true;

    buttonAddNewFolder.addEventListener('click', async () => {

        let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
        const selectedFolderPath = await window.fileExplorerApi.showFolderSelectDialog();

        requestBody["symbolFolderPath"] = `/Home/Test/`;
        requestBody["userFolderPath"] = selectedFolderPath;

        postJSON('http://localhost:1234/addNewFolder', requestBody);
    });
  });
  