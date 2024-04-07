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

    const buttonAddNewFolder = document.getElementById('button-add-new-folder');

    buttonAddNewFolder.addEventListener('click', async () => {

        let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
        const selectedFolderPath = await window.fileExplorerApi.showFolderSelectDialog();

        requestBody["symbolFolderPath"] = `/Home/Test/`;
        requestBody["userFolderPath"] = selectedFolderPath;

        postJSON('http://localhost:1234/addNewFolder', requestBody);
    });
  });
  