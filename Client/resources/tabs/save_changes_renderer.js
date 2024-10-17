document.addEventListener("DOMContentLoaded", async (event) => {
    let newAddedJson = await fetchJSON(`http://localhost:1234/newAddedList`);
    let deletedJson = await fetchJSON(`http://localhost:1234/deletedList`);
    let updatedJson = await fetchJSON(`http://localhost:1234/updatedFileList`);
  
    console.log(`newAdded: ${JSON.stringify(newAddedJson, null, 2)}`);
    console.log(`deleted: ${JSON.stringify(deletedJson, null, 2)}`);
    console.log(`updated: ${JSON.stringify(updatedJson, null, 2)}`);

    let treeStatus = {};
    let folderPathSet = new Set();

    for(currentFolder of newAddedJson.folders) {
      folderPathSet.add(currentFolder);
      treeStatus[currentFolder] = "NEW";
    }

    for(currentFolder in newAddedJson.files) {
      folderPathSet.add(currentFolder);
      newAddedJson.files[currentFolder].forEach(fileName => {treeStatus[currentFolder + fileName] = "NEW"});
    }

    for(currentFolder of deletedJson.folders) {
      folderPathSet.add(currentFolder);
      treeStatus[currentFolder] = "DELETED";
    }

    for(currentFolder in deletedJson.files) {
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

    for(path of folderPathList) {
      tree[path] = [];

      if(path in newAddedJson.files)
        tree[path].push(...newAddedJson.files[path]);

      if(path in deletedJson.files)
        tree[path].push(...deletedJson.files[path]);

      if(path in updatedJson)
        tree[path].push(...updatedJson[path]);

    }

    console.log(`tree: ${JSON.stringify(tree, null, 2)}`);
    console.log(`treeStatus: ${JSON.stringify(treeStatus, null, 2)}`);
});


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