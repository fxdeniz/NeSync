import MonitorApi from "../rest_api/MonitorApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {
  let buttonRefresh = document.getElementById(`button-refresh`);
  let buttonSave = document.getElementById('button-save');

  buttonSave.addEventListener('click', async (event) => {
    const commitMessage = document.getElementById(`input-commit-message`).value;
    await window.fmState.setCommitMessage(commitMessage);
    window.router.routeToSaveChanges();
  });

  buttonRefresh.addEventListener(`click`, async (event) => {
    displayFileSystemStatus(buttonRefresh, buttonSave);
  });

  await displayFileSystemStatus(buttonRefresh, buttonSave);
});


async function displayFileSystemStatus(buttonRefresh, buttonSave) {
  buttonRefresh.disabled = true;
  buttonSave.disabled = true;
  displayAlertDiv("Analyzing file system changes, please wait...");

  let monitorApi = new MonitorApi('localhost', 1234);

  let newAddedJson = await monitorApi.getNewAddedList();
  let deletedJson = await monitorApi.getDeletedList();
  let updatedJson = await monitorApi.getUpdatedFileList();

  await window.fmState.setNewAddedJson(newAddedJson);
  await window.fmState.setDeletedJson(deletedJson);
  await window.fmState.setUpdatedJson(updatedJson);

  console.log(`newAdded: ${JSON.stringify(newAddedJson, null, 2)}`);
  console.log(`deleted: ${JSON.stringify(deletedJson, null, 2)}`);
  console.log(`updated: ${JSON.stringify(updatedJson, null, 2)}`);

  let treeStatus = {};
  let folderPathSet = new Set();

  for(const currentFolder of newAddedJson.folders) {
    folderPathSet.add(currentFolder);
    treeStatus[currentFolder] = "NEW";
  }

  for(const currentFolder in newAddedJson.files) {
    folderPathSet.add(currentFolder);
    newAddedJson.files[currentFolder].forEach(fileName => {treeStatus[currentFolder + fileName] = "NEW"});
  }

  for(const currentFolder of deletedJson.folders) {
    folderPathSet.add(currentFolder);
    treeStatus[currentFolder] = "DELETED";
  }

  for(const currentFolder in deletedJson.files) {
    folderPathSet.add(currentFolder);
    deletedJson.files[currentFolder].forEach(fileName => {treeStatus[currentFolder + fileName] = "DELETED"});
  }

  Object.keys(updatedJson).forEach(key => {
    folderPathSet.add(key);
    updatedJson[key].forEach(fileName => {treeStatus[key + fileName] = "UPDATED"});
  });

  let folderPathList = Array.from(folderPathSet);
  folderPathList.sort((a, b) => a.length - b.length);

  let tree = {};

  for(const path of folderPathList) {
    tree[path] = [];

    if(path in newAddedJson.files)
      tree[path].push(...newAddedJson.files[path]);

    if(path in deletedJson.files)
      tree[path].push(...deletedJson.files[path]);

    if(path in updatedJson)
      tree[path].push(...updatedJson[path]);
  }

  displayAccordion(tree, treeStatus);

  console.log(`tree: ${JSON.stringify(tree, null, 2)}`);
  console.log(`treeStatus: ${JSON.stringify(treeStatus, null, 2)}`);

  closeAlertDiv();
  buttonRefresh.disabled = false;

  const isAnyFileUpdated = Object.keys(updatedJson).length > 0;
  const isAnyNewFileAdded = Object.keys(newAddedJson.files).length > 0;
  const isAnyFileDeleted = Object.keys(deletedJson.files).length > 0;
  const isAnyNewFolderAdded = newAddedJson.folders.length > 0;
  const isAnyFolderDeleted = deletedJson.folders.length > 0;

  if(isAnyFileUpdated || isAnyNewFileAdded || isAnyFileDeleted || isAnyNewFolderAdded || isAnyFolderDeleted)
    buttonSave.disabled = false;
}


function displayAlertDiv(alertMessage) {
  let divAlert = document.createElement("div");
  divAlert.id = "div-alert";
  divAlert.className = "alert alert-warning text-center";
  divAlert.setAttribute("role", "alert");
  divAlert.textContent = alertMessage;

  let container = document.getElementById("content-container");
  let referenceNode = container.querySelector(".row");

  container.insertBefore(divAlert, referenceNode);
}


function closeAlertDiv() {
  let divAlert = document.getElementById("div-alert");
  divAlert.remove();
}


function displayAccordion(tree, treeStatus) {
  if (document.getElementById('accordion-tree') !== null)
    document.getElementById('accordion-tree').remove();

  let collapseIndex = 0;

  let accordion = document.createElement('div');
  accordion.classList.add('accordion');
  accordion.setAttribute('id', `accordion-tree`);

  document.getElementById('content-container').appendChild(accordion);

  for(const folderPath in tree) {

    let accordionItemHeader = document.createElement('h2');
    accordionItemHeader.classList.add('accordion-header');

    let accordionButton = document.createElement('button');
    accordionButton.classList.add('accordion-button');
    accordionButton.setAttribute('type', 'button');
    accordionButton.setAttribute('data-bs-toggle', 'collapse');
    accordionButton.setAttribute('data-bs-target', `#collapse${collapseIndex}`);
    accordionButton.setAttribute('aria-expanded', 'true');
    accordionButton.setAttribute('aria-controls', `collapse${collapseIndex}`);

    if(folderPath in treeStatus)
      accordionButton.textContent = `${folderPath} (${treeStatus[folderPath].toLowerCase()})`;
    else 
      accordionButton.textContent = folderPath;

    accordionItemHeader.appendChild(accordionButton);

    let accordionBodyTopDiv = document.createElement('div');
    accordionBodyTopDiv.setAttribute('id', `collapse${collapseIndex}`);
    accordionBodyTopDiv.classList.add('accordion-collapse');
    accordionBodyTopDiv.classList.add('collapse');
    accordionBodyTopDiv.classList.add('show');
    accordionBodyTopDiv.setAttribute('data-bs-parent', `#dynamic${collapseIndex}`);

    let accordionBodyDiv = document.createElement('div');
    accordionBodyDiv.classList.add('accordion-body');

    if(tree[folderPath].length === 0)
      accordionBodyDiv.textContent = 'No files in this folder.';
    else {
      let ulItem = document.createElement('ul');
      accordionBodyDiv.appendChild(ulItem);

      tree[folderPath].forEach(fileName => {let liItem = document.createElement('li');

                                            if(folderPath + fileName in treeStatus)
                                              liItem.textContent = `${fileName} (${treeStatus[folderPath + fileName].toLowerCase()})`;
                                            else
                                              liItem.textContent = fileName;

                                            ulItem.appendChild(liItem);
      });
    }

    accordionBodyTopDiv.appendChild(accordionBodyDiv);

    let accordionItem = document.createElement('div');
    accordionItem.classList.add('accordion-item');
    accordionItem.appendChild(accordionItemHeader);
    accordionItem.appendChild(accordionBodyTopDiv);

    accordion.appendChild(accordionItem);

    collapseIndex += 1;
  }
}