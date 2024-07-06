document.addEventListener("DOMContentLoaded", async (event) => {
  let buttonRefresh = document.getElementById(`button-refresh`);

  buttonRefresh.addEventListener(`click`, async (event) => {
    let newAddedJson = await fetchJSON(`http://localhost:1234/newAddedList`);
    let deletedJson = await fetchJSON(`http://localhost:1234/deletedList`);
    let updatedJson = await fetchJSON(`http://localhost:1234/updatedFileList`);
  
    console.log(`newAdded: ${JSON.stringify(newAddedJson, null, 2)}`);
    console.log(`deleted: ${JSON.stringify(deletedJson, null, 2)}`);
    console.log(`updated: ${JSON.stringify(updatedJson, null, 2)}`);

    let folderPathSet = new Set();

    for(currentFolder of newAddedJson.folders) {
      folderPathSet.add(currentFolder);
    }

    for(currentFolder in newAddedJson.files) {
      folderPathSet.add(currentFolder);
    }

    for(currentFolder of deletedJson.folders) {
      folderPathSet.add(currentFolder);
    }

    for(currentFolder in deletedJson.files) {
      folderPathSet.add(currentFolder);
    }

    Object.keys(updatedJson).forEach(key => {
      folderPathSet.add(key);
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

    if (document.getElementById('accordion-tree') !== null)
      document.getElementById('accordion-tree').remove();

    let collapseIndex = 0;

    let accordion = document.createElement('div');
    accordion.classList.add('accordion');
    accordion.setAttribute('id', `accordion-tree`);

    document.getElementById('content-container').appendChild(accordion);

    for(folderPath in tree) {

      let accordionItemHeader = document.createElement('h2');
      accordionItemHeader.classList.add('accordion-header');
  
      let accordionButton = document.createElement('button');
      accordionButton.classList.add('accordion-button');
      accordionButton.setAttribute('type', 'button');
      accordionButton.setAttribute('data-bs-toggle', 'collapse');
      accordionButton.setAttribute('data-bs-target', `#collapse${collapseIndex}`);
      accordionButton.setAttribute('aria-expanded', 'true');
      accordionButton.setAttribute('aria-controls', `collapse${collapseIndex}`);
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
      accordionBodyDiv.textContent = 'Dynamic Text Here';

      if(tree[folderPath].length !== 0) {
        let ulItem = document.createElement('ul');
        accordionBodyDiv.appendChild(ulItem);

        tree[folderPath].forEach(item => {let liItem = document.createElement('li');
                                          liItem.textContent = item;
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
    
  });

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