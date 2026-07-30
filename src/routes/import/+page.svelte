<script lang="ts">
	import { importService } from '$lib/services/import';
	import { library } from '$lib/stores/library.svelte';

	async function chooseFolder() {
		try {
			await importService.chooseLibraryFolder();
		} catch (err) {
			console.error(err);
		}
	}
</script>

<h1>Import</h1>

<button onclick={chooseFolder}>
	Choose Library Folder
</button>

{#if library.items.length}
	<h2>Selected</h2>

	<ul>
		{#each library.items as item}
			<li>{item.path}</li>
		{/each}
	</ul>
{/if}
