<script lang="ts">
	let { children, onclick } = $props();
</script>

<button {onclick}>
	{@render children()}
</button>

<style>
	button {
		background: var(--accent);
		color: white;
		border: none;
		border-radius: 999px;
		padding: 0.6rem 1rem;
		cursor: pointer;
	}
</style>
